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

#include "geo.h"

class TransportCatalogue {
public:
	struct Stop {
		std::string name;
		Coordinates coordinate;
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
	void AddBus(const std::pair<Bus, std::vector<std::string>>& bus_stops);

	const std::pair<Stop*, std::set<Bus*>> FindStopWithBuses(const std::string& name) const;
	Stop* FindStop(const std::string& name);
	const Bus* FindBus(const std::string& name) const;
	void SetDistance(const std::string& from, const std::string& to, int l);
	int GetDistance(Stop* st1, Stop* st2) const;

private:
	std::deque<Stop> stops_;
	std::deque<Bus> buses_;
	std::map<Stop*, std::set<Bus*>> stop_to_buses_;
	std::unordered_map<PairStops, int, PairStopsHasher> dist_;
};