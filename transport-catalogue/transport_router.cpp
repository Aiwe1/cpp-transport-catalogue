#include "transport_router.h"

std::pair<graph::VertexId, graph::VertexId> TransportRouter::GetPairId(std::string_view from, std::string_view to) {
	std::pair<graph::VertexId, graph::VertexId> from_to;

	graph::VertexId id = 0;
	const auto& index_stops = tc_.GetIndexStops();
	for (const auto& stop : index_stops) {
		if (stop.first == from) {
			from_to.first = id;
		}
		if (stop.first == to) {
			from_to.second = id;
		}
		++id;
	}
	return from_to;
}

void TransportRouter::MakeRouter(const RouterSettings& router_settings) {
	if (router_ptr_) {
		return;
	}

	using namespace graph;
	using namespace std;
	const auto& index_stops = tc_.GetIndexStops();
	DirectedWeightedGraph<double> graph_(index_stops.size());

	AddStopsToGraph(graph_, router_settings);
	AddBusesToGraph(graph_, router_settings);

	graph_ptr_ = make_shared<DirectedWeightedGraph<double>>(std::move(graph_));
	Router<double> router_(*graph_ptr_);
	router_ptr_ = make_shared<Router<double>>(move(router_));
}

void TransportRouter::AddStopsToGraph(graph::DirectedWeightedGraph<double>& graph_, const RouterSettings& router_settings) {
	using namespace graph;
	using namespace std;

	const auto& index_stops = tc_.GetIndexStops();
	VertexId id = 0;
	for (const auto& stop : index_stops) {
		Edge<double> e;
		e.from = id;
		e.to = id;
		e.weight = router_settings.bus_wait_time;
		EdgeId e_id = graph_.AddEdge(e); /// Возвращает ID ребра
		EdgeInfo edge_info{ stop.second , stop.second, nullptr };
		stop_id[stop.second] = id;
		edge_to_info_.insert({ e_id, edge_info });
		++id;
	}
}

void TransportRouter::AddBusesToGraph(graph::DirectedWeightedGraph<double>& graph_, const RouterSettings& router_settings) {
	using namespace graph;
	using namespace std;

	const auto& index_buses = tc_.GetIndexBuses();

	for (const auto& [bus_name, bus] : index_buses) {
		for (int i = 0; i < bus->stops.size() - 1; ++i) {
			for (int j = i + 1; j < bus->stops.size(); ++j) {
				{
					Edge<double> e;
					e.from = stop_id.at(bus->stops.at(i));
					e.to = stop_id.at(bus->stops.at(j));

					int length = 0;
					for (int k = i; k < j; ++k) {
						length += tc_.GetDistance(bus->stops.at(k), bus->stops.at(k + 1));
					}

					e.weight = static_cast<double>(router_settings.bus_wait_time)
						+ static_cast<double>(length) / router_settings.bus_velocity;
					EdgeId e_id = graph_.AddEdge(e);
					EdgeInfo edge_info{ bus->stops.at(i) , bus->stops.at(j), bus,  e.weight, j - i };
					edge_to_info_.insert({ e_id, edge_info });
				}
				if (!bus->is_round_) {
					Edge<double> e;
					e.from = stop_id.at(bus->stops.at(j));
					e.to = stop_id.at(bus->stops.at(i));

					int length = 0;
					for (int k = j; k > i; --k) {
						length += tc_.GetDistance(bus->stops.at(k), bus->stops.at(k - 1));
					}

					e.weight = static_cast<double>(router_settings.bus_wait_time)
						+ static_cast<double>(length) / router_settings.bus_velocity;
					EdgeId e_id = graph_.AddEdge(e);
					EdgeInfo edge_info{ bus->stops.at(j) , bus->stops.at(i), bus,  e.weight, j - i };
					edge_to_info_.insert({ e_id, edge_info });
				}
			}
		}
	}
}