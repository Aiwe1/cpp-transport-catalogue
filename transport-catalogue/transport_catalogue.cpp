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

	return dist_.at({ st2, st1 });
}

int TransportCatalogue::GetDistanceForward(Stop* st1, Stop* st2) const {
	if (dist_.find({ st1, st2 }) != dist_.end()) 
		return dist_.at({ st1, st2 });
	
	return -1;
}
std::shared_ptr<graph::Router<double>> TransportCatalogue::MakeRouter(RouterSettings& router_settings) {
	/////////////////////////////////////////
	if (router_) {
		return router_;
	}

	using namespace graph;
	using namespace std;
	DirectedWeightedGraph<double> g(stops_.size());

	//map<Stop*, VertexId> stop_id;
	VertexId id = 0;
	for (const auto& stop : index_stops_) {
		Edge<double> e;
		e.from = id;
		e.to = id;
		e.weight = router_settings.bus_wait_time;
		EdgeId e_id = g.AddEdge(e); /// Возвращает ID ребра
		//stop_id.emplace(stop.second, id);
		EdgeInfo edge_info{ stop.second , stop.second, nullptr };
		stop_id[stop.second] = id;
		edge_to_info_.insert({ e_id, edge_info });
		++id;
	}

	for (const auto& [n, bus] : index_buses_) {
		for (int i = 0; i < bus->stops.size() - 1; ++i) {
			for (int j = i + 1; j < bus->stops.size(); ++j) {
				{
					Edge<double> e;
					e.from = stop_id.at(bus->stops.at(i));
					e.to = stop_id.at(bus->stops.at(j));

					int length = 0;
					for (int k = i; k < j; ++k) {
						length += GetDistance(bus->stops.at(k), bus->stops.at(k + 1));
					}

					e.weight = static_cast<double>(router_settings.bus_wait_time)
						+ static_cast<double>(length) / router_settings.bus_velocity;
					EdgeId e_id = g.AddEdge(e);
					EdgeInfo edge_info{ bus->stops.at(i) , bus->stops.at(j), bus,  e.weight, j - i };
					edge_to_info_.insert({ e_id, edge_info });
				}
				if (!bus->is_round_) {
					Edge<double> e;
					e.from = stop_id.at(bus->stops.at(j));
					e.to = stop_id.at(bus->stops.at(i));

					int length = 0;
					for (int k = j; k > i; --k) {
						length += GetDistance(bus->stops.at(k), bus->stops.at(k - 1));
					}

					e.weight = static_cast<double>(router_settings.bus_wait_time)
						+ static_cast<double>(length) / router_settings.bus_velocity;
					EdgeId e_id = g.AddEdge(e);
					EdgeInfo edge_info{ bus->stops.at(j) , bus->stops.at(i), bus,  e.weight, j - i };
					edge_to_info_.insert({ e_id, edge_info });
				}
			}
		}
		//if (!bus->is_round_)
		//{
		//	for (int i = bus->stops.size() - 1; i > 0; --i) {
		//		for (int j = i - 1; j >= 0; --j) {
		//			Edge<double> e;
		//			e.from = stop_id.at(bus->stops.at(i));
		//			e.to = stop_id.at(bus->stops.at(j));
		//
		//			int length = 0;
		//			for (int k = i; k > j; --k) {
		//				length += GetDistance(bus->stops.at(k), bus->stops.at(k - 1));
		//			}
		//
		//			e.weight = static_cast<double>(router_settings.bus_wait_time)
		//				+ static_cast<double>(length) / router_settings.bus_velocity;
		//			EdgeId e_id = g.AddEdge(e);
		//			EdgeInfo edge_info{ bus->stops.at(i) , bus->stops.at(j), bus,  e.weight, i - j };
		//			edge_to_info_.insert({ e_id, edge_info });
		//		}
		//	}
		//}
	}

	graph_ = make_shared<DirectedWeightedGraph<double>>(std::move(g));
	Router<double> r(*graph_);
	router_ = make_shared<Router<double>>(move(r));
	return router_;
}