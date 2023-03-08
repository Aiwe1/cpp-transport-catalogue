#pragma once

#include "router.h"
#include "json.h"
#include "transport_catalogue.h"

struct RouterSettings {
	constexpr static double METER_PER_MIN = 1000.0 / 60.0;
	RouterSettings() {}
	RouterSettings(const json::Dict& d) {
		using namespace json;

		bus_wait_time = static_cast<double>(d.at("bus_wait_time").AsInt());
		bus_velocity = d.at("bus_velocity").AsDouble() * METER_PER_MIN;
	}
	RouterSettings(double bus_w_t, double bus_v) : bus_wait_time(bus_w_t), bus_velocity(bus_v) {
	}
	RouterSettings& SetFrom(std::string_view s) {
		from_ = s;
		return *this;
	}
	RouterSettings& SetTo(std::string_view s) {
		to_ = s;
		return *this;
	}

	double bus_wait_time; // minute
	double bus_velocity; // meter / min
	std::string_view from_;
	std::string_view to_;
};

class TransportRouter {
public:	
	explicit TransportRouter(const TransportCatalogue& tc) : tc_(tc) {}

	struct EdgeInfo {
		TransportCatalogue::Stop* from = nullptr;
		TransportCatalogue::Stop* to = nullptr;
		TransportCatalogue::Bus* bus = nullptr;
		double weight = 0.0;
		int span_count = 0;
	};
	EdgeInfo& GetEdgeInfo(graph::EdgeId e_id) {
		return edge_to_info_.at(e_id);
	}
	std::pair<graph::VertexId, graph::VertexId> GetPairId(std::string_view from, std::string_view to);

	void AddStopsToGraph(graph::DirectedWeightedGraph<double>& graph_, const RouterSettings& router_settings);
	void AddBusesToGraph(graph::DirectedWeightedGraph<double>& graph_, const RouterSettings& router_settings);

	//std::shared_ptr<graph::Router<double>> 
	void MakeRouter(const RouterSettings& router_settings);

	auto GetRoteInfo(RouterSettings router_settings) {
		MakeRouter(router_settings);

		auto from_to = GetPairId(router_settings.from_, router_settings.to_);

		auto res = router_ptr_->BuildRoute(from_to.first, from_to.second);
		return res;
	}

private:
	std::shared_ptr<graph::DirectedWeightedGraph<double>> graph_ptr_;
	std::shared_ptr<graph::Router<double>> router_ptr_;

	std::map<graph::EdgeId, EdgeInfo> edge_to_info_;
	std::map<TransportCatalogue::Stop*, graph::VertexId> stop_id;

	const TransportCatalogue& tc_;
};