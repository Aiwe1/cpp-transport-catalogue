/*
#include "stat_reader.h"
#include "domain.h"

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
*/