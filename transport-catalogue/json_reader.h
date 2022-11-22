#pragma once

#include <iostream>
#include <string>
#include <istream>
#include <deque>
#include <unordered_map>
#include <algorithm>

#include "geo.h"
#include "transport_catalogue.h"
#include "json.h"
#include "map_renderer.h"
/*
void PrintBus(const std::string& name, TransportCatalogue& tc, std::ostream& out) {
	using namespace std;
	//Bus 751 : not found
	const auto& bus = tc.FindBus(name);
	out << "Bus "s << name << ": "s;
	if (!bus) {
		out << "not found"s;
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
		out << bus->stops.size() << " stops on route, " << Uniq.size() << " unique stops, "s <<
			length << " route length, "s << setprecision(6) << l << " curvature";
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
		out << bus->stops.size() * 2 - 1 << " stops on route, " << Uniq.size() << " unique stops, "s <<
			length << " route length, "s << setprecision(6) << l << " curvature";
	}
}

void PrintStop(const std::string& name, TransportCatalogue& tc, std::ostream& out) {
	using namespace std;
	//Stop Samara: not found
	//Stop Prazhskaya : no buses
	//Stop Biryulyovo Zapadnoye : buses 256 828
	const auto st = tc.FindStop(name);
	out << "Stop "s << name << ": "s;
	if (!st) {
		out << "not found"s;
		return;
	}
	const auto stop_to_buses = tc.FindStopWithBuses(name);

	if (stop_to_buses.buses.size() == 0) {
		out << "no buses"s;
	}
	else {
		out << "buses";
		vector<TransportCatalogue::Bus*> v;
		v.reserve(stop_to_buses.buses.size());
		for (const auto& b : stop_to_buses.buses) {
			v.push_back(b);
		}
		sort(v.begin(), v.end(), [](TransportCatalogue::Bus* l, TransportCatalogue::Bus* r)
			{return l->name < r->name; });
		for (const auto& b : v) {
			out << ' ' << b->name;
		}
	}
}

void ReadRequests(TransportCatalogue& tc, std::istream& in, std::ostream& out) {
	using namespace std;
	string s;

	int N = 0;
	getline(in, s);
	N = stoi(s);

	for (int i = 0; i < N; ++i) {
		getline(in, s);
		s = s.substr(s.find_first_not_of(' '));
		if (s[0] == 'B') {
			PrintBus(DeleteSpace(s.substr(4)), tc, out);
			out << endl;
		}
		if (s[0] == 'S') {
			PrintStop(DeleteSpace(s.substr(5)), tc, out);
			out << endl;
		}
	} 
}

std::string DeleteSpace(std::string s) {
    int i = 0, j = s.size() - 1;
    for (; i < s.size(); ++i)
        if (s[i] != ' ') {
            break;
        }
    for (; j >= 0; --j)
        if (s[j] != ' ') {
            break;
        }
    s = s.substr(i, j - i + 1);
    return s;
}
std::pair<TransportCatalogue::Stop, std::string> SplitStop(const std::string& text) {
    using namespace std;

    TransportCatalogue::Stop stop;
    string s1, s2, dis;
    int i = text.find(':');
    stop.name = text.substr(5, i - 5);
    ++i;
    for (; text[i] != ','; ++i)
        s1.push_back(text[i]);
    ++i;
    for (++i; i < text.size() && text[i] != ','; ++i)
        s2.push_back(text[i]);

    stop.coordinate = { stod(s1), stod(s2) };
    if (i < text.size() - 1)
        dis = text.substr(i);
    return { stop, dis };
}
TransportCatalogue::BusToStops SplitBus(const std::string& text) {
    using namespace std;

    TransportCatalogue::BusToStops BusToStops;
    BusToStops.bus.is_round_ = false;

    int i = text.find(':');
    BusToStops.bus.name = text.substr(4, i - 4);
    ++i;

    for (; i < text.size(); ++i) {
        int x = i;
        while (text[i] != '>' && text[i] != '-' && i < text.size()) {
            ++i;
        }
        if (text[i] == '>')
            BusToStops.bus.is_round_ = true;

        BusToStops.stops.push_back(DeleteSpace(text.substr(x, i - x)));
    }

    return BusToStops;
}

std::vector<std::pair<int, std::string>> SplitDistance(std::string& text) {
    using namespace std;
    vector<pair<int, std::string>> res;
    pair<int, std::string> p;
    int i = 1;
    int size_ = text.size();
    for (; i < size_; ++i) {
        string s1, s2;
        while (text[i] != 'm') {
            s1.push_back(text[i]);
            ++i;
        }
        i += 4;
        while (text[i] != ',' && i < size_) {
            s2.push_back(text[i]);
            ++i;
        }
        ++i;
        s2 = DeleteSpace(s2);
        res.push_back({ stoi(s1), s2 });
    }

    return res;
}*/
void BusJson(const std::string& name, TransportCatalogue& tc, json::Dict& dict);
void StopJson(const std::string& name, TransportCatalogue& tc, json::Dict& dict);
void PrintJson(TransportCatalogue& tc, json::Dict& a, std::ostream& os);
void ReadAll(TransportCatalogue& tc, std::istream& is, std::ostream& os);