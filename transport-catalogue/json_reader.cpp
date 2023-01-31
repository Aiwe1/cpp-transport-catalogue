#include <map>

#include "json_reader.h"

using namespace json;
using namespace std;
using namespace graph;

void PutBusToJson(const std::string& name, TransportCatalogue& tc, json::Builder& b) {
    const auto& bus = tc.FindBus(name);
    //out << "Bus "s << name << ": "s;
    if (!bus) {
        b.Key("error_message"s).Value("not found"s);
        return;
    }
    else if (bus->is_round_) {
        //Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature	
        set<TransportCatalogue::Stop*> uniq;
        double curvature = 0.0;
        int length = 0;

        for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
            uniq.insert(bus->stops.at(i));
            curvature += ComputeDistance(bus->stops.at(i)->coordinate, bus->stops.at(i + 1)->coordinate);

            length += tc.GetDistance(bus->stops.at(i), bus->stops.at(i + 1));
        }
        curvature = static_cast<double>(length) / curvature;

        b.Key("curvature"s).Value(curvature);
        b.Key("route_length"s).Value(length);
        b.Key("stop_count"s).Value(static_cast<int>(bus->stops.size()));
        b.Key("unique_stop_count"s).Value(static_cast<int>(uniq.size()));
    }
    else {
        set<TransportCatalogue::Stop*> uniq;
        double curvature = 0.0;
        int length = 0;
        size_t i = 0;

        for (; i < bus->stops.size() - 1; ++i) {
            uniq.insert(bus->stops.at(i));
            curvature += ComputeDistance(bus->stops.at(i)->coordinate, bus->stops.at(i + 1)->coordinate) * 2.0;
            length += tc.GetDistance(bus->stops.at(i), bus->stops.at(i + 1));
        }
        uniq.insert(bus->stops.at(i));

        for (; i > 0; --i) {
            length += tc.GetDistance(bus->stops.at(i), bus->stops.at(i - 1));
        }
        curvature = static_cast<double>(length) / curvature;

        b.Key("curvature"s).Value(curvature);
        b.Key("route_length"s).Value(length);
        b.Key("stop_count"s).Value(static_cast<int>(bus->stops.size() * 2 - 1));
        b.Key("unique_stop_count"s).Value(static_cast<int>(uniq.size()));
    }
}

void PutStopToJson(const std::string& name, TransportCatalogue& tc, json::Builder& b) {
    const auto stop = tc.FindStop(name);

    if (!stop) {
        b.Key("error_message"s).Value("not found"s);
        return;
    }
    b.Key("buses"s);
    b.StartArray();
    const auto stop_to_buses = tc.FindStopWithBuses(name);

    if (stop_to_buses.buses.size() > 0) {
        vector<TransportCatalogue::Bus*> buses;
        buses.reserve(stop_to_buses.buses.size());
        for (const auto& bus : stop_to_buses.buses) {
            buses.push_back(bus);
        }
        sort(buses.begin(), buses.end(), [](TransportCatalogue::Bus* lhs, TransportCatalogue::Bus* rhs)
            {return lhs->name < rhs->name; });
        for (const auto& bus : buses) {
            b.Value(bus->name);
        }
    }
    b.EndArray();
}

void PutRouteToJson(RouterSettings& router_settings, TransportCatalogue& tc, json::Builder& b) {
    using namespace graph;
    using EdgeInfo = TransportCatalogue::EdgeInfo;
    //auto g = tc.MakeGraph(router_settings);
    //Router router(*g);
    auto router = tc.MakeRouter(router_settings);

    auto from_to = tc.GetFromAndToId(router_settings.from_, router_settings.to_);

    auto res = router->BuildRoute(from_to.first, from_to.second);

    //   На Данный Момент не совпадает на 6 минут(ровно одно время ожидания)
    if (!res) {
        // "error_message": "not found"
        b.Key("error_message"s).Value("not found");
        return;
    }

    b.Key("total_time"s).Value(res.value().weight); ////   каждый раз прибавляется время ожидания?
    
    //auto& asdasd = res.value().edges;

    b.Key("items"s).StartArray();
    for (auto e_id = res.value().edges.begin(); e_id != res.value().edges.end(); ++e_id) {
        EdgeInfo edge_info = tc.GetEdgeInfo(*e_id);
        b.StartDict();
        b.Key("type"s).Value("Wait"s);
        b.Key("time"s).Value(router_settings.bus_wait_time);
        b.Key("stop_name"s).Value(edge_info.from->name);
        b.EndDict();

        if (edge_info.bus) {
            b.StartDict();
            b.Key("type"s).Value("Bus"s);
            b.Key("time"s).Value(edge_info.weight - router_settings.bus_wait_time);
            b.Key("span_count"s).Value(edge_info.span_count);
            b.Key("bus").Value(edge_info.bus->name);

            b.EndDict();

            //std::cout << "bus:  " << edge_info.bus->name << ": " << edge_info.weight << endl;
        }
    }
    b.EndArray();
    //std::cout << res.value().weight << endl;
    //std::cout << "-------------" << endl;
}

void PrintJson(RenderSettings &rs, RouterSettings &router_settings, TransportCatalogue& tc, json::Dict& request, std::ostream& os) {
    if (request.find("stat_requests"s) == request.end())
        return;

    const Array& stat = request.at("stat_requests"s).AsArray();

    json::Builder b;
    b.StartArray();

    for (const auto& unit_ : stat) {
        const auto& unit = unit_.AsDict();
        if (unit.at("type").AsString() == "Stop") {
            b.StartDict();
            b.Key("request_id"s).Value(unit.at("id"s).AsInt());
            PutStopToJson(unit.at("name"s).AsString(), tc, b);
            b.EndDict();
        }
        else if (unit.at("type").AsString() == "Bus") {
            b.StartDict();
            b.Key("request_id"s).Value(unit.at("id"s).AsInt());
            PutBusToJson(unit.at("name"s).AsString(), tc, b);
            b.EndDict();
        }
        else if (unit.at("type").AsString() == "Map") {
            b.StartDict();
            b.Key("request_id"s).Value(unit.at("id"s).AsInt());

            ostringstream os;
            MakeSVG(rs, tc, os);

            b.Key("map"s).Value(os.str());
            b.EndDict();
        }
        else if (unit.at("type").AsString() == "Route") {
            b.StartDict();
            b.Key("request_id"s).Value(unit.at("id"s).AsInt());

            PutRouteToJson(router_settings.SetFrom(unit.at("from").AsString()).SetTo(unit.at("to").AsString()), tc, b);
            b.EndDict();
        }
    }
    json::Print(Document{ b.EndArray().Build()}, os);
}

void AddBusesStops(TransportCatalogue& tc, const json::Array& base) {
    unordered_map<string, vector<pair<int, string>>> distances;
    for (const auto& unit_ : base) {
        const auto& unit = unit_.AsDict();
        if (unit.at("type").AsString() == "Stop") {

            TransportCatalogue::Stop stop;
            stop.name = unit.at("name").AsString();
            stop.coordinate.lat = unit.at("latitude").AsDouble();
            stop.coordinate.lng = unit.at("longitude").AsDouble();
            tc.AddStop(stop);

            if (unit.find("road_distances") != unit.end()) {
                vector<pair<int, string>> dist_to_name;
                for (auto& dist : unit.at("road_distances").AsDict()) {
                    dist_to_name.push_back(pair(dist.second.AsInt(), dist.first));
                }
                distances.insert({ stop.name, dist_to_name });
            }
        }
    }
    for (const auto& unit_ : base) {
        const auto& unit = unit_.AsDict();
        if (unit.at("type").AsString() == "Bus") {
            TransportCatalogue::BusToStops bus_to_stops;

            bus_to_stops.bus.name = unit.at("name").AsString();
            bus_to_stops.bus.is_round_ = unit.at("is_roundtrip").AsBool();

            for (auto& stop : unit.at("stops").AsArray()) {
                bus_to_stops.stops.push_back(stop.AsString());

            }
            tc.AddBus(bus_to_stops);
        }
    }
    for (auto& [from, dis_to_stops] : distances) {
        for (auto& [dis, to] : dis_to_stops) {
            tc.SetDistance(from, to, dis);
        }

    }
}

void ReadAll(TransportCatalogue& tc, std::istream& is, std::ostream& os) {
    Dict a = Load(is).GetRoot().AsDict();
    // Add
    AddBusesStops(tc, a.at("base_requests"s).AsArray());
    

    //routing_settings    
    RenderSettings render_settings(a.at("render_settings"s).AsDict());

    // render settings
    RouterSettings router_settings(a.at("routing_settings"s).AsDict());
    
    //MakeSVG(rs, tc, os);
    
    //test
    // Requests
    PrintJson(render_settings, router_settings, tc, a, os);
}