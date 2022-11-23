#include "map_renderer.h"

RenderSettings::RenderSettings(const json::Dict& d) {
	using namespace json;

	width = d.at("width").AsDouble();
	height = d.at("height").AsDouble();

	padding = d.at("padding").AsDouble();

	line_width = d.at("line_width").AsDouble();
	stop_radius = d.at("stop_radius").AsDouble();

	bus_label_font_size = d.at("bus_label_font_size").AsInt();

	bus_label_offset.x = d.at("bus_label_offset").AsArray()[0].AsDouble();
	bus_label_offset.y = d.at("bus_label_offset").AsArray()[1].AsDouble();

	stop_label_font_size = d.at("stop_label_font_size").AsInt();

	stop_label_offset.x = d.at("stop_label_offset").AsArray()[0].AsDouble();
	stop_label_offset.y = d.at("stop_label_offset").AsArray()[1].AsDouble();

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

void MakeSVG(RenderSettings& rs, TransportCatalogue& tc, std::ostream& os) {
	using namespace svg;
	
	auto buses = tc.GetAllBuses();
	std::sort(buses.begin(), buses.end(), [](TransportCatalogue::Bus& r, TransportCatalogue::Bus& l) {
												return r.name < l.name; });
	//std::set<TransportCatalogue::Bus> buses = tc.GetAllBuses();
	std::vector<geo::Coordinates> all_coords;
	std::map<std::string, geo::Coordinates> stops_sorted;
	for (const auto& bus : buses) {
		for (const auto& stop : bus.stops) {
			all_coords.push_back(stop->coordinate);
			stops_sorted.insert({ stop->name, stop->coordinate });
		}
	}
	SphereProjector SP(all_coords.begin(), all_coords.end(),
		rs.width, rs.height, rs.padding);

	std::vector<svg::Circle> circles;
	std::vector<svg::Text> stop_names;
	for (const auto& [s, p] : stops_sorted) {
		Circle c;
		c.SetRadius(rs.stop_radius).SetFillColor("white"s).SetCenter(SP(p));
		circles.push_back(c);

		Text t1;
		t1.SetData(s).SetPosition(SP(p)).SetOffset(rs.stop_label_offset).SetFontSize(rs.stop_label_font_size).
			SetFontFamily("Verdana"s);
		Text t2 = t1;
		t1.SetFillColor(rs.underlayer_color).SetStrokeColor(rs.underlayer_color).
			SetStrokeWidth(rs.underlayer_width).SetStrokeLineCap(StrokeLineCap::ROUND).
			SetStrokeLineJoin(StrokeLineJoin::ROUND);
		t2.SetFillColor("black"s);
		stop_names.push_back(t1);
		stop_names.push_back(t2);
	}

	svg::Document svg_doc;
	std::vector<svg::Text> bus_names;

	// polylines
	for (int i = 0; i < (int)buses.size(); ++i) {
		if (buses[i].stops.size() < 2)
			continue;
		Polyline line;
		Text bus_name1, bus_name2;
		
		bus_name1.SetFontSize(rs.bus_label_font_size).SetOffset(rs.bus_label_offset).
			SetFontFamily("Verdana"s).SetFontWeight("bold"s).SetData(buses[i].name).
			SetPosition(SP(buses[i].stops[0]->coordinate));
		bus_name2 = bus_name1;
		bus_name1.SetFillColor(rs.underlayer_color).SetStrokeColor(rs.underlayer_color).
			SetStrokeWidth(rs.underlayer_width).SetStrokeLineCap(StrokeLineCap::ROUND).
			SetStrokeLineJoin(StrokeLineJoin::ROUND);
		bus_name2.SetFillColor(rs.color_palette[i % (int)rs.color_palette.size()]);
		bus_names.push_back(bus_name1);
		bus_names.push_back(bus_name2);
		{
			line.SetFillColor(NoneColor);
			line.SetStrokeColor(rs.color_palette[i % (int)rs.color_palette.size()]);
			line.SetStrokeWidth(rs.line_width);
			line.SetStrokeLineCap(StrokeLineCap::ROUND);
			line.SetStrokeLineJoin(StrokeLineJoin::ROUND);
		}
		for (auto& c : buses[i].stops) {
			Point p = SP(c->coordinate);
			line.AddPoint(p);
		}
		if (!buses[i].is_round_) {
			for (int j = buses[i].stops.size() - 2; j >= 0; --j) {
				Point p = SP(buses[i].stops[j]->coordinate);
				line.AddPoint(p);
			}
			if (buses[i].stops[0]->name != buses[i].stops.back()->name)
			{
				bus_name1.SetPosition(SP(buses[i].stops.back()->coordinate));
				bus_name2.SetPosition(SP(buses[i].stops.back()->coordinate));
				bus_names.push_back(bus_name1);
				bus_names.push_back(bus_name2);
			}
		}
		svg_doc.Add(std::move(line));
	}

	for (auto& b : bus_names) {
		svg_doc.Add(std::move(b));
	}
	for (auto& c : circles) {
		svg_doc.Add(std::move(c));
	}
	for (auto& s : stop_names) {
		svg_doc.Add(std::move(s));
	}
	svg_doc.Render(os);
}