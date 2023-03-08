#include "serialization.h"

void SaveTo(std::ostream& output, RenderSettings& render_settings, RouterSettings& router_settings, const json::Array& base) {
    transport_catalogue_serialize::TransportCatalogue tcs;

    //transport_catalogue_serialize::RouterSettings router;
    tcs.mutable_router_settings()->set_bus_velocity(router_settings.bus_velocity);
    tcs.mutable_router_settings()->set_bus_wait_time(router_settings.bus_wait_time);

    // RenderSettings
    tcs.mutable_render_settings()->set_width(render_settings.width);
    tcs.mutable_render_settings()->set_height(render_settings.height);
    tcs.mutable_render_settings()->set_padding(render_settings.padding);
    tcs.mutable_render_settings()->set_line_width(render_settings.line_width);
    tcs.mutable_render_settings()->set_stop_radius(render_settings.stop_radius);
    tcs.mutable_render_settings()->set_bus_label_font_size(render_settings.bus_label_font_size);
    tcs.mutable_render_settings()->set_stop_label_font_size(render_settings.stop_label_font_size);
    tcs.mutable_render_settings()->set_underlayer_width(render_settings.underlayer_width);

    //bus_label_offset
    tcs.mutable_render_settings()->mutable_bus_label_offset()->set_x(render_settings.bus_label_offset.x);
    tcs.mutable_render_settings()->mutable_bus_label_offset()->set_y(render_settings.bus_label_offset.y);

    //Point stop_label_offset
    tcs.mutable_render_settings()->mutable_stop_label_offset()->set_x(render_settings.stop_label_offset.x);
    tcs.mutable_render_settings()->mutable_stop_label_offset()->set_y(render_settings.stop_label_offset.y);

    //Color underlayer_color
    svg::Color_norm col;
    col = svg::GetColor(col, render_settings.underlayer_color);
    tcs.mutable_render_settings()->mutable_underlayer_color()->set_what_(col.what_);
    switch (col.what_)
    {
    case 1:
        tcs.mutable_render_settings()->mutable_underlayer_color()->set_str(col.str_);
        break;
    case 2:
        tcs.mutable_render_settings()->mutable_underlayer_color()->mutable_rgb()->set_red(col.rgb_.red);
        tcs.mutable_render_settings()->mutable_underlayer_color()->mutable_rgb()->set_green(col.rgb_.green);
        tcs.mutable_render_settings()->mutable_underlayer_color()->mutable_rgb()->set_blue(col.rgb_.blue);
        break;
    case 3:
        tcs.mutable_render_settings()->mutable_underlayer_color()->mutable_rgba()->set_red(col.rgba_.red);
        tcs.mutable_render_settings()->mutable_underlayer_color()->mutable_rgba()->set_green(col.rgba_.green);
        tcs.mutable_render_settings()->mutable_underlayer_color()->mutable_rgba()->set_blue(col.rgba_.blue);
        tcs.mutable_render_settings()->mutable_underlayer_color()->mutable_rgba()->set_opacity(col.rgba_.opacity);
        break;
    default:
        break;
    }

    for (auto& c : render_settings.color_palette) {
        transport_catalogue_serialize::Color color_ser;
        svg::Color_norm col1;
        col1 = svg::GetColor(col1, c);
        color_ser.set_what_(col1.what_);
        switch (col1.what_)
        {
        case 1:
        {
            color_ser.set_str(col1.str_);
            break;
        }
        case 2:
        {
            color_ser.mutable_rgb()->set_red(col1.rgb_.red);
            color_ser.mutable_rgb()->set_green(col1.rgb_.green);
            color_ser.mutable_rgb()->set_blue(col1.rgb_.blue);
            break;
        }
        case 3:
        {
            color_ser.mutable_rgba()->set_red(col1.rgba_.red);
            color_ser.mutable_rgba()->set_green(col1.rgba_.green);
            color_ser.mutable_rgba()->set_blue(col1.rgba_.blue);
            color_ser.mutable_rgba()->set_opacity(col1.rgba_.opacity);
            break;
        }
        default:
        {
            break;
        }
        }
        *tcs.mutable_render_settings()->add_color_palette() = std::move(color_ser);
    }

    // TC
    for (const auto& unit_ : base) {
        const auto& unit = unit_.AsDict();
        if (unit.at("type").AsString() == "Stop") {
            transport_catalogue_serialize::Stop st;
            st.set_name(unit.at("name").AsString());
            st.mutable_coordinate()->set_lat(unit.at("latitude").AsDouble());
            st.mutable_coordinate()->set_lng(unit.at("longitude").AsDouble());

            if (unit.find("road_distances") != unit.end()) {
                for (auto& dist : unit.at("road_distances").AsDict()) {
                    transport_catalogue_serialize::RoadDistance rd;
                    rd.set_name(dist.first);
                    rd.set_dist(dist.second.AsInt());
                    *st.add_road_distances() = std::move(rd);
                }
            }

            *tcs.add_stops() = std::move(st);
        }
        else {
            transport_catalogue_serialize::Bus bus;
            bus.set_name(unit.at("name").AsString());
            bus.set_is_round_(unit.at("is_roundtrip").AsBool());
            for (auto& stop : unit.at("stops").AsArray()) {
                bus.add_stops(stop.AsString());
                //st.set_name(stop.AsString());  
            }
            *tcs.add_buses() = std::move(bus);
        }
    }
    tcs.SerializeToOstream(&output);
}

void Deserialize(std::istream& in, TransportCatalogue& tc,
    RenderSettings& render_settings, RouterSettings& router_settings) {
    transport_catalogue_serialize::TransportCatalogue tcs;

    tcs.ParseFromIstream(&in);
    //RouterSettings 
    router_settings.bus_wait_time = tcs.router_settings().bus_wait_time();
    router_settings.bus_velocity = tcs.router_settings().bus_velocity();

    //RenderSettings 
    const auto& rs = tcs.render_settings();

    // RenderSettings
    render_settings.width = rs.width();
    render_settings.height = rs.height();
    render_settings.padding = rs.padding();
    render_settings.line_width = rs.line_width();
    render_settings.stop_radius = rs.stop_radius();
    render_settings.bus_label_font_size = rs.bus_label_font_size();
    render_settings.stop_label_font_size = rs.stop_label_font_size();
    render_settings.underlayer_width = rs.underlayer_width();
    //bus_label_offset
    render_settings.bus_label_offset.x = rs.bus_label_offset().x();
    render_settings.bus_label_offset.y = rs.bus_label_offset().y();
    //Point stop_label_offset
    render_settings.stop_label_offset.x = rs.stop_label_offset().x();
    render_settings.stop_label_offset.y = rs.stop_label_offset().y();
    //Color underlayer_color
    switch (rs.underlayer_color().what_())
    {
    case 1:
        render_settings.underlayer_color = rs.underlayer_color().str();
        break;
    case 2:
    {
        svg::Rgb rgb_;
        rgb_.blue = rs.underlayer_color().rgb().blue();
        rgb_.red = rs.underlayer_color().rgb().red();
        rgb_.green = rs.underlayer_color().rgb().green();
        render_settings.underlayer_color = rgb_;
        break;
    }
    case 3:
    {
        svg::Rgba rgba_;
        rgba_.blue = rs.underlayer_color().rgba().blue();
        rgba_.red = rs.underlayer_color().rgba().red();
        rgba_.green = rs.underlayer_color().rgba().green();
        rgba_.opacity = rs.underlayer_color().rgba().opacity();
        render_settings.underlayer_color = rgba_;
        break;
    }
    default:
        break;
    }

    for (auto& c : rs.color_palette()) {
        svg::Color col;
        switch (c.what_())
        {
        case 1:
        {
            col = c.str();
            break;
        }
        case 2:
        {
            svg::Rgb rgb_;
            rgb_.blue = c.rgb().blue();
            rgb_.red = c.rgb().red();
            rgb_.green = c.rgb().green();
            col = rgb_;
            break;
        }
        case 3:
        {
            svg::Rgba rgba_;
            rgba_.blue = c.rgba().blue();
            rgba_.red = c.rgba().red();
            rgba_.green = c.rgba().green();
            rgba_.opacity = c.rgba().opacity();
            col = rgba_;
            break;
        }
        default:
            break;
        }
        render_settings.color_palette.push_back(std::move(col));
    }
    // TC  const json::Array& base
    std::unordered_map<std::string, std::vector<std::pair<int, std::string>>> distances;
    for (const auto& stop_ : tcs.stops()) {
        TransportCatalogue::Stop stop;
        stop.name = stop_.name(); // unit.at("name").AsString();
        stop.coordinate.lat = stop_.coordinate().lat(); //unit.at("latitude").AsDouble();
        stop.coordinate.lng = stop_.coordinate().lng();// unit.at("longitude").AsDouble();
        tc.AddStop(stop);

        if (stop_.road_distances_size() != 0) {
            std::vector<std::pair<int, std::string>> dist_to_name;

            for (auto& dist : stop_.road_distances()) {
                dist_to_name.push_back(std::pair(dist.dist(), dist.name()));
            }
            distances.insert({ stop.name, dist_to_name });
        }
    }
    for (const auto& bus_ : tcs.buses()) {
        //const auto& unit = unit_.AsDict();
        TransportCatalogue::BusToStops bus_to_stops;

        bus_to_stops.bus.name = bus_.name(); //unit.at("name").AsString();
        bus_to_stops.bus.is_round_ = bus_.is_round_(); // unit.at("is_roundtrip").AsBool();

        for (auto& stop : bus_.stops()) {
            bus_to_stops.stops.push_back(stop);
        }
        tc.AddBus(bus_to_stops);
    }
    for (auto& [from, dis_to_stops] : distances) {
        for (auto& [dis, to] : dis_to_stops) {
            tc.SetDistance(from, to, dis);
        }
    }
}
