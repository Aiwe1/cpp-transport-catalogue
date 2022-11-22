#include "json_reader.h"

void BusJson(const std::string& name, TransportCatalogue& tc, json::Dict& dict) {
    using namespace std;
    using namespace json;

    const auto& bus = tc.FindBus(name);
    //Dict dict;
    //out << "Bus "s << name << ": "s;
    if (!bus) {
        dict.insert({ {"error_message"s}, {"not found"s } });
        //out << "not found"s;
        return;// dict;
    }
    else if (bus->is_round_ == true) {
        //Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature	
        set<TransportCatalogue::Stop*> Uniq;
        double l = 0.0;
        int length = 0;
        int i = 0;

        for (; i < bus->stops.size() - 1; ++i) {
            Uniq.insert(bus->stops.at(i));
            l += ComputeDistance(bus->stops.at(i)->coordinate, bus->stops.at(i + 1)->coordinate);

            length += tc.GetDistance(bus->stops.at(i), bus->stops.at(i + 1));
        }
        l = static_cast<double>(length) / l;
        //out << bus->stops.size() << " stops on route, " << Uniq.size() << " unique stops, "s <<
        //    length << " route length, "s << setprecision(6) << l << " curvature";
        dict.insert({ "curvature"s, {l} });
        dict.insert({ "route_length"s, {length} });
        dict.insert({ "stop_count"s, {(int)bus->stops.size()} });
        dict.insert({ "unique_stop_count"s, {(int)Uniq.size() } });
    }
    else {
        set<TransportCatalogue::Stop*> Uniq;
        double l = 0.0;
        int length = 0;
        int i = 0;
        //Bus 750 : 7 stops on route, 3 unique stops, 27400 route length, 1.30853 curvature	
        for (; i < bus->stops.size() - 1; ++i) {
            Uniq.insert(bus->stops.at(i));
            l += ComputeDistance(bus->stops.at(i)->coordinate, bus->stops.at(i + 1)->coordinate) * 2.0;
            length += tc.GetDistance(bus->stops.at(i), bus->stops.at(i + 1));
        }
        Uniq.insert(bus->stops.at(i));

        for (; i > 0; --i) {
            length += tc.GetDistance(bus->stops.at(i), bus->stops.at(i - 1));
        }
        l = static_cast<double>(length) / l;
        //out << bus->stops.size() * 2 - 1 << " stops on route, " << Uniq.size() << " unique stops, "s <<
        //    length << " route length, "s << setprecision(6) << l << " curvature";
        dict.insert({ "curvature"s, {l} });
        dict.insert({ "route_length"s, {length} });
        dict.insert({ "stop_count"s, {(int)(bus->stops.size() * 2 - 1) } });
        dict.insert({ "unique_stop_count"s, {(int)Uniq.size()} });
    }

}
void StopJson(const std::string& name, TransportCatalogue& tc, json::Dict& dict) {
    using namespace std;
    using namespace json;

    const auto st = tc.FindStop(name);
    //out << "Stop "s << name << ": "s;
    if (!st) {
        //out << "not found"s;
        dict.insert({ {"error_message"s}, {"not found"s } });
        return;
    }
    Array arr;
    const auto stop_to_buses = tc.FindStopWithBuses(name);

    if (stop_to_buses.buses.size() == 0) {
        //out << "no buses"s;
    }
    else {
        //out << "buses";
        vector<TransportCatalogue::Bus*> v;
        v.reserve(stop_to_buses.buses.size());
        arr.reserve(stop_to_buses.buses.size());
        for (const auto& b : stop_to_buses.buses) {
            v.push_back(b);
        }
        sort(v.begin(), v.end(), [](TransportCatalogue::Bus* l, TransportCatalogue::Bus* r)
            {return l->name < r->name; });
        for (const auto& b : v) {
            //out << ' ' << b->name;
            arr.push_back({ b->name });
        }
    }

    dict.insert({ {"buses"s }, { arr }});
}

void PrintJson(TransportCatalogue& tc, json::Dict& a, std::ostream& os) {
    using namespace json;
    using namespace std;

    if (a.find("stat_requests"s) == a.end())
        return;

    const Array& stat = a.at("stat_requests"s).AsArray();

    Array arr;
    for (const auto& unit_ : stat) {
        //Dict dict;
        const auto& unit = unit_.AsMap();
        if (unit.at("type").AsString() == "Stop") {
            Dict dict; // = StopJson(unit.at("name"s).AsString(), tc);
            dict.insert({ "request_id"s, unit.at("id"s).AsInt() });

            //dict.insert({ "buses"s, StopJson(unit.at("name"s).AsString(), tc) });
            StopJson(unit.at("name"s).AsString(), tc, dict);
            arr.push_back(dict);
        }
        else if (unit.at("type").AsString() == "Bus") {
            Dict dict;
            dict.insert({ "request_id"s, unit.at("id").AsInt() });
            BusJson(unit.at("name"s).AsString(), tc, dict);

            arr.push_back(dict);
        }
    }
    json::Print(Document{ arr }, os);
}

void ReadAll(TransportCatalogue& tc, std::istream& is, std::ostream& os) {
    using namespace json;
    using namespace std;

    Dict a = Load(is).GetRoot().AsMap();
    // Add
    {
        const Array& base = a.at("base_requests"s).AsArray();

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
    // render settings
    RenderSettings render_settings(a.at("render_settings"s).AsMap());
    MakeSVG(render_settings, tc, os);
    
    //test
    // Requests
    //PrintJson(tc, a, os);
}