#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const Stop& stop) {
	stops_.push_back(stop);
	index_stops_.insert({ stops_.back().name, &stops_.back() });
}

void TransportCatalogue::AddBus(const std::pair<Bus, std::vector<std::string>>& bus_stops) {
	Bus bus = bus_stops.first;
	std::vector<std::string> stops = bus_stops.second;

	std::set<Stop*> v;

	for (auto& stop : stops) {
		auto it = std::find_if(stops_.begin(), stops_.end(), [&](Stop& s) {return s.name == stop; });
		bus.stops.push_back(&(*it));
		v.insert(&(*it));
		//Почему указатель &bus_  каждый раз одинаковый? 
		//Из-за этого не могу в этом же цикле stop_to_buses_ заполниить
	}
	buses_.push_back(std::move(bus));
	for (auto& it : v) {
		//stop_to_buses_.insert({ &(*it), {&(buses_.back())} });
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

const std::pair<TransportCatalogue::Stop*, std::set<TransportCatalogue::Bus*>> 
			TransportCatalogue::FindStopWithBuses(const std::string& name) const {
	const auto& res = std::find_if(stop_to_buses_.begin(), stop_to_buses_.end(),
			[&name](const auto& m) {return m.first->name == name; });

	if (res == stop_to_buses_.end())
	{
		std::pair<TransportCatalogue::Stop*, std::set<TransportCatalogue::Bus*>> p;
		return p;
	}
	
	return *res;
}
const TransportCatalogue::Bus* TransportCatalogue::FindBus(const std::string& name) const {
	auto res = std::find_if(buses_.begin(), buses_.end(), [&name](const Bus& b) {return b.name == name; });
	if (res == buses_.end())
		return nullptr;
	return &(*res);
}

void TransportCatalogue::SetDistance(const std::string& from, const std::string& to, int l) {
	PairStops p;
	p.from = FindStop(from);
	p.to = FindStop(to);
	dist_[p] = l;
}

int TransportCatalogue::GetDistance(Stop* st1, Stop* st2) const {
	if (dist_.find({ st1, st2 }) != dist_.end())
		return dist_.at({ st1, st2 });
	return dist_.at({ st2, st1 });
}