#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const Stop& stop) {
	stops_.push_back(stop);
	index_stops_.insert({ stops_.back().name, &stops_.back() });
}

void TransportCatalogue::AddBus(const BusToStops& bus_stops) {
	Bus bus = bus_stops.bus;
	//std::vector<std::string> stops = std::move(bus_stops.stops);

	std::set<Stop*> v;

	for (auto& stop : bus_stops.stops) {
		auto p_stop = FindStop(stop);
		bus.stops.push_back(p_stop);
		v.insert(p_stop);
	}
	buses_.push_back(std::move(bus));
	for (auto& it : v) {
		stop_to_buses_[&(*it)].insert({ &(buses_.back()) });
	} 
	index_buses_.insert({ buses_.back().name, &buses_.back() });
}

TransportCatalogue::Stop* TransportCatalogue::FindStop(const std::string& name) {
	auto res = std::find_if(stops_.begin(), stops_.end(), [&name](const Stop& b) {return b.name == name; });
	if (res == stops_.end())
		return nullptr;
	return &(*res);
}

const TransportCatalogue::StopToBuses TransportCatalogue::FindStopWithBuses(const std::string& name) const {
	const auto& it = std::find_if(stop_to_buses_.begin(), stop_to_buses_.end(),
			[&name](const auto& m) {return m.first->name == name; });
	TransportCatalogue::StopToBuses res;
	if (it == stop_to_buses_.end()){
		return res;
	}
	res.stop = it->first;
	res.buses = it->second;
	return res;
}
const TransportCatalogue::Bus* TransportCatalogue::FindBus(const std::string& name) const {
	auto res = std::find_if(buses_.begin(), buses_.end(), [&name](const Bus& b) {return b.name == name; });
	if (res == buses_.end())
		return nullptr;
	return &(*res);
}

void TransportCatalogue::SetDistance(const std::string& from, const std::string& to, int dist) {
	PairStops p;
	p.from = FindStop(from);
	p.to = FindStop(to);
	dist_[p] = dist;
}

int TransportCatalogue::GetDistance(Stop* st1, Stop* st2) const {
	if (dist_.find({ st1, st2 }) != dist_.end())
		return dist_.at({ st1, st2 });
	// не понял, о каком цикле речь, после if что ли скобки всегда надо ставить?
	return dist_.at({ st2, st1 });
}