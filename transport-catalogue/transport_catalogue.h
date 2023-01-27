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

	std::unique_ptr<graph::DirectedWeightedGraph<double>>& MakeGraph(RouterSettings& router_settings) {
		/////////////////////////////////////////
		if (graph_) {
			return graph_;
		}

		using namespace graph;
		using namespace std;
		DirectedWeightedGraph<double> g(stops_.size());

		map<Stop*, VertexId> stop_id;
		VertexId id = 0;
		for (const auto& stop : index_stops_) {
		    Edge<double> e;
		    e.from = id;
		    e.to = id;
			e.weight = router_settings.bus_wait_time;
		    g.AddEdge(e); /// Возвращает ID ребра, мб придётся юзать
			//stop_id.emplace(stop.second, id);
			stop_id[stop.second] = id;
		    ++id;
		}
		for (const auto& dist : dist_) {
			Edge<double> e;
			e.from = stop_id.at(dist.first.from);
			e.to = stop_id.at(dist.first.to);
			e.weight = static_cast<double>(router_settings.bus_wait_time)
				+ static_cast<double>(dist.second) / router_settings.bus_velocity;
			g.AddEdge(e); /// Возвращает ID ребра, мб придётся юзать
			//stop_id.emplace(stop.second, id);
		}
		////////////////////
		graph_ = make_unique<DirectedWeightedGraph<double>>(std::move(g));
		return graph_;
	}
private:
	std::deque<Stop> stops_;
	std::deque<Bus> buses_;
	std::unordered_map<Stop*, std::set<Bus*>> stop_to_buses_;
	std::unordered_map<PairStops, int, PairStopsHasher> dist_;
	std::unordered_map<std::string_view, Stop*, std::hash<std::string_view>> index_stops_;
	std::unordered_map<std::string_view, Bus*, std::hash<std::string_view>> index_buses_;

	std::unique_ptr<graph::DirectedWeightedGraph<double>> graph_;
};