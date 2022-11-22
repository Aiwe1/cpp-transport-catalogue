#include "map_renderer.h"

RenderSettings::RenderSettings(const json::Dict& d) {
	using namespace json;

	width = d.at("width").AsDouble();
	height = d.at("height").AsDouble();

	padding = d.at("padding").AsDouble();

	line_width = d.at("line_width").AsDouble();
	stop_radius = d.at("stop_radius").AsDouble();

	bus_label_font_size = d.at("bus_label_font_size").AsInt();

	bus_label_offset.dx = d.at("bus_label_offset").AsArray()[0].AsDouble();
	bus_label_offset.dy = d.at("bus_label_offset").AsArray()[1].AsDouble();

	stop_label_font_size = d.at("stop_label_font_size").AsInt();

	stop_label_offset.dx = d.at("stop_label_offset").AsArray()[0].AsDouble();
	stop_label_offset.dy = d.at("stop_label_offset").AsArray()[1].AsDouble();

	underlayer_width = d.at("underlayer_width").AsDouble();

	if (d.at("underlayer_color").IsString()) {
		underlayer_color = d.at("underlayer_color").AsString();
	}
	else if (d.at("underlayer_color").AsArray().size() == 3) {
		svg::Rgb col;
		col.red = d.at("underlayer_color").AsArray()[0].AsInt();
		col.green = d.at("underlayer_color").AsArray()[1].AsInt();
		col.blue = d.at("underlayer_color").AsArray()[2].AsInt();
		underlayer_color = col;
	}
	else {
		svg::Rgba col;
		col.red = d.at("underlayer_color").AsArray()[0].AsInt();
		col.green = d.at("underlayer_color").AsArray()[1].AsInt();
		col.blue = d.at("underlayer_color").AsArray()[2].AsInt();
		col.opacity = d.at("underlayer_color").AsArray()[3].AsDouble();
		underlayer_color = col;
	}

	const json::Array& arr = d.at("color_palette").AsArray();

	for (const auto& a : arr) {
		if (a.IsString()) {
			color_palette.push_back( a.AsString() );
		}
		else if (a.AsArray().size() == 3) {
			svg::Rgb col;
			col.red = a.AsArray()[0].AsInt();
			col.green = a.AsArray()[1].AsInt();
			col.blue = a.AsArray()[2].AsInt();
			color_palette.push_back(col);
		}
		else {
			svg::Rgba col;
			col.red = a.AsArray()[0].AsInt();
			col.green = a.AsArray()[1].AsInt();
			col.blue = a.AsArray()[2].AsInt();
			col.opacity = a.AsArray()[3].AsDouble();
			color_palette.push_back(col);
		}
	}

}

void MakeSVG(RenderSettings& render_settings, TransportCatalogue& tc, std::ostream& os) {
	using namespace svg;
	
	auto& buses = tc.GetAllBuses();
	std::sort(buses.begin(), buses.end(), [](TransportCatalogue::Bus& r, TransportCatalogue::Bus& l) {
												return r.name < l.name; });
	//std::set<TransportCatalogue::Bus> buses = tc.GetAllBuses();
	std::vector<geo::Coordinates> all_coords;
	for (const auto& bus : buses) {
		for (const auto& stop : bus.stops) {
			all_coords.push_back(stop->coordinate);
		}
	}

	SphereProjector SP(all_coords.begin(), all_coords.end(),
		render_settings.width, render_settings.height, render_settings.padding);

	svg::Document svg_doc;

	for (int i = 0; i < buses.size(); ++i) {
		Polyline line;
		line.SetFillColor(NoneColor);

		line.SetStrokeColor(render_settings.color_palette[i]);
		line.SetStrokeWidth(render_settings.line_width);
		line.SetStrokeLineCap(StrokeLineCap::ROUND);
		line.SetStrokeLineJoin(StrokeLineJoin::ROUND);

		for (auto& c : buses[i].stops) {
			Point p = SP(c->coordinate);
			line.AddPoint(p);
		}
		if (!buses[i].is_round_) {
			for (int j = buses[i].stops.size() - 2; j >= 0; --j) {
				Point p = SP(buses[i].stops[j]->coordinate);
				line.AddPoint(p);
			}
		}
		svg_doc.Add(line);
	}
	svg_doc.Render(os);

	return;
}