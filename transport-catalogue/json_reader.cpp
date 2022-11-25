#include "json_reader.h"

using namespace json;
using namespace std;

void PutBusToJson(const std::string& name, TransportCatalogue& tc, json::Dict& dict) {
    const auto& bus = tc.FindBus(name);
    //Dict dict;
    //out << "Bus "s << name << ": "s;
    if (!bus) {
        dict.insert({ {"error_message"s}, {"not found"s } });
        //out << "not found"s;
        return;// dict;
    }
    else if (bus->is_round_) {
        //Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature	
        set<TransportCatalogue::Stop*> uniq;
        double curvature = 0.0;
        int length = 0;

        for (size_t i = 0; i < bus->stops.size() - 1; ++i) { 
            // не понял про начало с 1. Ну будет bus->stops.size() нулём, ну выйдем сразу из цикла, какая разница?
            uniq.insert(bus->stops.at(i));
            curvature += ComputeDistance(bus->stops.at(i)->coordinate, bus->stops.at(i + 1)->coordinate);

            length += tc.GetDistance(bus->stops.at(i), bus->stops.at(i + 1));
        }
        curvature = static_cast<double>(length) / curvature;
        //out << bus->stops.size() << " stops on route, " << Uniq.size() << " unique stops, "s <<
        //    length << " route length, "s << setprecision(6) << l << " curvature";
        dict.insert({ "curvature"s, {curvature} });
        dict.insert({ "route_length"s, {length} });
        dict.insert({ "stop_count"s, {static_cast<int>(bus->stops.size())} });
        dict.insert({ "unique_stop_count"s, {static_cast<int>(uniq.size()) } });
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
        //out << bus->stops.size() * 2 - 1 << " stops on route, " << Uniq.size() << " unique stops, "s <<
        //    length << " route length, "s << setprecision(6) << l << " curvature";
        dict.insert({ "curvature"s, {curvature} });
        dict.insert({ "route_length"s, {length} });
        dict.insert({ "stop_count"s, {static_cast<int>(bus->stops.size() * 2 - 1) } });
        dict.insert({ "unique_stop_count"s, {static_cast<int>(uniq.size())} });
    }

}
void PutStopToJson(const std::string& name, TransportCatalogue& tc, json::Dict& dict) {
    const auto stop = tc.FindStop(name);
    //out << "Stop "s << name << ": "s;
    if (!stop) {
        //out << "not found"s;
        dict.insert({ {"error_message"s}, {"not found"s } });
        return;
    }
    Array arr;
    const auto stop_to_buses = tc.FindStopWithBuses(name);

    if (stop_to_buses.buses.size() > 0) {
        vector<TransportCatalogue::Bus*> buses; 
        buses.reserve(stop_to_buses.buses.size());
        arr.reserve(stop_to_buses.buses.size());
        for (const auto& bus : stop_to_buses.buses) {
            buses.push_back(bus);
        }
        sort(buses.begin(), buses.end(), [](TransportCatalogue::Bus* lhs, TransportCatalogue::Bus* rhs)
            {return lhs->name < rhs->name; });
        for (const auto& bus : buses) {
            //out << ' ' << b->name;
            arr.push_back({ bus->name });
        }
    }

    dict.insert({ {"buses"s }, { arr }});
}

void PrintJson(RenderSettings &rs, TransportCatalogue& tc, json::Dict& request, std::ostream& os) {
    if (request.find("stat_requests"s) == request.end())
        return;

    const Array& stat = request.at("stat_requests"s).AsArray();

    Array arr;
    for (const auto& unit_ : stat) {
        //Dict dict;
        const auto& unit = unit_.AsMap();
        if (unit.at("type").AsString() == "Stop") {
            Dict dict; // = StopJson(unit.at("name"s).AsString(), tc);
            dict.insert({ "request_id"s, unit.at("id"s).AsInt() });

            //dict.insert({ "buses"s, StopJson(unit.at("name"s).AsString(), tc) });
            PutStopToJson(unit.at("name"s).AsString(), tc, dict);
            arr.push_back(dict);
        }
        else if (unit.at("type").AsString() == "Bus") {
            Dict dict;
            dict.insert({ "request_id"s, unit.at("id").AsInt() });
            PutBusToJson(unit.at("name"s).AsString(), tc, dict);

            arr.push_back(dict);
        }
        else if (unit.at("type").AsString() == "Map") {
            Dict dict;
            dict.insert({ "request_id"s, unit.at("id").AsInt() });
            ostringstream os;
            MakeSVG(rs, tc, os);
            dict.insert({ "map"s,os.str() });

            arr.push_back(dict);
        }
    }
    json::Print(Document{ arr }, os);
}

void AddBusesStops(TransportCatalogue& tc, const json::Array& base) {
    unordered_map<string, vector<pair<int, string>>> distances;
    for (const auto& unit_ : base) {
        const auto& unit = unit_.AsMap();
        if (unit.at("type").AsString() == "Stop") {

            TransportCatalogue::Stop stop;
            stop.name = unit.at("name").AsString();
            stop.coordinate.lat = unit.at("latitude").AsDouble();
            stop.coordinate.lng = unit.at("longitude").AsDouble();
            tc.AddStop(stop);

            if (unit.find("road_distances") != unit.end()) {
                vector<pair<int, string>> dist_to_name;
                for (auto& dist : unit.at("road_distances").AsMap()) {
                    dist_to_name.push_back(pair(dist.second.AsInt(), dist.first));
                }
                distances.insert({ stop.name, dist_to_name });
            }
        }
    }
    for (const auto& unit_ : base) {
        const auto& unit = unit_.AsMap();
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
    Dict a = Load(is).GetRoot().AsMap();
    // Add
    AddBusesStops(tc, a.at("base_requests"s).AsArray());
    
    // render settings
    RenderSettings rs(a.at("render_settings"s).AsMap());

    //MakeSVG(rs, tc, os);
    
    //test
    // Requests
    PrintJson(rs, tc, a, os);
}