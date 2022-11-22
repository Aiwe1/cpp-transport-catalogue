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