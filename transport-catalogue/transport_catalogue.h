#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <iomanip>
#include <sstream>
#include <set>
#include <algorithm>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <string_view>
#include <memory>

#include "geo.h"
#include "graph.h"
#include "transport_router.h"

class TransportCatalogue {
public:
	struct Stop {
		std::string name;
		geo::Coordinates coordinate;
	};

	struct Bus {
		std::string name;
		std::deque<Stop*> stops;
		bool is_round_ = false;
	};
	struct PairStops
	{
		Stop* from;
		Stop* to;

		bool operator==(const PairStops& other) const {
			return (from == other.from && to == other.to);
		}
	};
	struct PairStopsHasher {
		size_t operator()(const PairStops& stop) const {
			std::size_t h1 = std::hash<const void*>{}(stop.from);
			std::size_t h2 = std::hash<const void*>{}(stop.to);
			return h1 ^ (h2 << 1);
		}
	};
	void AddStop(const Stop& stop);

	struct BusToStops {
		Bus bus;
		std::vector<std::string> stops;
	};
	void AddBus(const BusToStops& bus_stops);

	struct StopToBuses {
		Stop* stop = nullptr;
		std::set<Bus*> buses;
	};
	const StopToBuses FindStopWithBuses(const std::string& name) const;
	Stop* FindStop(const std::string& name);
	const Bus* FindBus(const std::string& name) const;
	void SetDistance(const std::string& from, const std::string& to, int dist);
	int GetDistance(Stop* st1, Stop* st2) const;
	int GetDistanceForward(Stop* st1, Stop* st2) const;

	std::deque<Bus> GetAllBuses() const {
		return buses_;
	}

	struct EdgeInfo {
		Stop* from = nullptr;
		Stop* to = nullptr;
		Bus* bus = nullptr;
		double weight = 0.0;
	};

	std::shared_ptr<graph::DirectedWeightedGraph<double>> MakeGraph(RouterSettings& router_settings) {
		/////////////////////////////////////////
		if (graph_) {
			return graph_;
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
		    EdgeId e_id = g.AddEdge(e); /// ���������� ID �����
			//stop_id.emplace(stop.second, id);
			EdgeInfo edge_info{ stop.second , stop.second, nullptr};
			stop_id[stop.second] = id;
			edge_to_info_.insert({ e_id, edge_info });
		    ++id;
		}
		
		for (const auto& [n , bus] : index_buses_) {
			for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
				Edge<double> e;
				e.from = stop_id.at(bus->stops.at(i));
				e.to = stop_id.at(bus->stops.at(i + 1));

				int length = GetDistance(bus->stops.at(i), bus->stops.at(i + 1));
				e.weight = static_cast<double>(router_settings.bus_wait_time)
					+ static_cast<double>(length) / router_settings.bus_velocity;
				EdgeId e_id = g.AddEdge(e);
				EdgeInfo edge_info{ bus->stops.at(i) , bus->stops.at(i + 1), bus,  e.weight };
				edge_to_info_.insert({ e_id, edge_info });
			}

		}
		
		//for (const auto& dist : dist_) {
		//	Edge<double> e;
		//	e.from = stop_id.at(dist.first.from);
		//	e.to = stop_id.at(dist.first.to);
		//	e.weight = static_cast<double>(router_settings.bus_wait_time)
		//		+ static_cast<double>(dist.second) / router_settings.bus_velocity;
		//	EdgeId e_id = g.AddEdge(e); /// ���������� ID �����, �� ������� �����
		//	edge_to_vertex_id_.emplace(e_id, e);
		//	//stop_id.emplace(stop.second, id);
		//}
		////////////////////
		graph_ = make_shared<DirectedWeightedGraph<double>>(std::move(g));
		return graph_;
	}

	EdgeInfo& GetEdgeInfo(graph::EdgeId e_id) {
		return edge_to_info_.at(e_id);
	}

	std::pair<graph::VertexId, graph::VertexId> GetFromAndToId(std::string_view from, std::string_view to) {
		std::pair<graph::VertexId, graph::VertexId> from_to;
	
		graph::VertexId id = 0;
		for (const auto& stop : index_stops_) {
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

private:
	std::deque<Stop> stops_;
	std::deque<Bus> buses_;
	std::unordered_map<Stop*, std::set<Bus*>> stop_to_buses_;
	std::unordered_map<PairStops, int, PairStopsHasher> dist_;
	std::unordered_map<std::string_view, Stop*, std::hash<std::string_view>> index_stops_;
	std::unordered_map<std::string_view, Bus*, std::hash<std::string_view>> index_buses_;

	std::shared_ptr<graph::DirectedWeightedGraph<double>> graph_;

	std::map<graph::EdgeId, EdgeInfo> edge_to_info_;
	std::map<Stop*, graph::VertexId> stop_id;
};