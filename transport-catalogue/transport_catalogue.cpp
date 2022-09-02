#include "transport_catalogue.h"

void TransportCatalogue::AddStop(std::pair<std::string, Coordinates> stop) {
	stops_.push_back({ stop.first, stop.second });
}

void TransportCatalogue::AddBus(std::pair<std::pair<std::string, std::vector<std::string>>, bool> bus) {
	Bus bus_;
	bus_.Name = bus.first.first;
	bus_.is_round_ = bus.second;
	std::set<Stop*> v;

	for (auto& stop : bus.first.second) {
		auto it = std::find_if(stops_.begin(), stops_.end(), [&](Stop& s) {return s.Name == stop; });
		bus_.Stops.push_back(&(*it));
		v.insert(&(*it));
		//Почему указатель &bus_  каждый раз одинаковый? 
		//Из-за этого не могу в этом же цикле stop_to_buses_ заполниить
	}

	buses_.push_back(std::move(bus_));
	for (auto& it : v) {
		//stop_to_buses_.insert({ &(*it), {&(buses_.back())} });
		stop_to_buses_[&(*it)].insert({ &(buses_.back()) });
	}
}

TransportCatalogue::Stop* TransportCatalogue::FindStop(const std::string& name) {
	auto res = &(*std::find_if(stops_.begin(), stops_.end(), [&name](const Stop& st) {return st.Name == name; }));
	return res;
}
TransportCatalogue::Bus* TransportCatalogue::FindBus(const std::string& name) {
	auto res = &(*std::find_if(buses_.begin(), buses_.end(), [&name](const Bus& b) {return b.Name == name; }));
	return res;
}

std::string TransportCatalogue::GetInfoBus(const std::string& name) const {
	using namespace std;
	ostringstream os;
	//Bus 751 : not found
	const auto& bus = std::find_if(buses_.begin(), buses_.end(), [&name](const Bus& b) {return b.Name == name; });
	os << "Bus "s << name << ": "s;
	if (bus == buses_.end()) {
		os << "not found"s;
	}
	else if (bus->is_round_ == true) {
		//Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature	
		set<Stop*> Uniq;
		double l = 0.0;
		int length = 0;
		int i = 0;

		for (; i < bus->Stops.size() - 1; ++i) {
			Uniq.insert(bus->Stops.at(i));
			l += ComputeDistance(bus->Stops.at(i)->c, bus->Stops.at(i + 1)->c);

			length += GetDistance(bus->Stops.at(i), bus->Stops.at(i + 1));
		}
		l = static_cast<double>(length) / l;
		os << bus->Stops.size() << " stops on route, " << Uniq.size() << " unique stops, "s <<
			length << " route length, "s << setprecision(6) << l << " curvature";
	}
	else {
		set<Stop*> Uniq;
		double l = 0.0;
		int length = 0;
		int i = 0;
		string s = (*bus).Name;
		//Bus 750 : 7 stops on route, 3 unique stops, 27400 route length, 1.30853 curvature	
		for (; i < bus->Stops.size() - 1; ++i) {
			Uniq.insert(bus->Stops.at(i));
			l += ComputeDistance(bus->Stops.at(i)->c, bus->Stops.at(i + 1)->c) * 2.0;
			length += GetDistance(bus->Stops.at(i), bus->Stops.at(i + 1));
		}
		Uniq.insert(bus->Stops.at(i));

		for (; i > 0; --i) {
			length += GetDistance(bus->Stops.at(i), bus->Stops.at(i - 1));
		}
		l = static_cast<double>(length) / l;
		os << bus->Stops.size() * 2 - 1 << " stops on route, " << Uniq.size() << " unique stops, "s <<
			length << " route length, "s << setprecision(6) << l << " curvature";
	}

	return os.str();
}

std::string TransportCatalogue::GetInfoStop(const std::string& name) const {
	using namespace std;
	ostringstream os;
	//Stop Samara: not found
	//Stop Prazhskaya : no buses
	//Stop Biryulyovo Zapadnoye : buses 256 828
	os << "Stop "s << name << ": "s;

	if (stops_.end() == std::find_if(stops_.begin(), stops_.end(),
		[&name](const auto& st) {return st.Name == name; })) {
		os << "not found"s;
		return os.str();
	}
	const auto& it = std::find_if(stop_to_buses_.begin(), stop_to_buses_.end(),
		[&name](const auto& m) {return m.first->Name == name; });

	if (it == stop_to_buses_.end()) {
		os << "no buses"s;
	}
	else {
		os << "buses";
		vector<Bus*> v;
		v.reserve(it->second.size());
		for (const auto& b : it->second) {
			v.push_back(b);
		}
		sort(v.begin(), v.end(), [](Bus* l, Bus* r) {return l->Name < r->Name; });
		for (const auto& b : v) {
			os << ' ' << b->Name;
		}
	}

	return os.str();
}

void TransportCatalogue::AddDistance(std::unordered_map<std::string, std::vector<std::pair<int, std::string>>> dist) {
	//std::unordered_map<std::pair<Stop*, Stop*>, int, PairStopsHasher> dist_;
	//std::pair<Stop*, Stop*> p;

	for (auto& [st1, stops] : dist) {
		for (auto& [l, st2] : stops) {
			PairStops p;
			p.st1 = FindStop(st1);
			p.st2 = FindStop(st2);
			dist_[p] = l;
		}
	}
}

int TransportCatalogue::GetDistance(Stop* st1, Stop* st2) const {
	if (dist_.find({ st1, st2 }) != dist_.end())
		return dist_.at({ st1, st2 });
	//if (dist_.find({ st2, st1 }) != dist_.end())
	return dist_.at({ st2, st1 });
	//return 0;
}

void TransportCatalogue::PrintAll() {
	using namespace std;
	for (const auto& st : stops_) {
		cout << st.Name << ": "s << setprecision(6) << st.c.lat << ", "s << setprecision(6) << st.c.lng << endl;
	}
	for (const auto& bus : buses_) {
		cout << endl << bus.Name << ": "s;
		for (const auto& s : bus.Stops) {
			cout << s->Name << " - "s;
		}
	}
}