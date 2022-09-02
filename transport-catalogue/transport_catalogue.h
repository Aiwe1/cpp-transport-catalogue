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
		std::string Name;
		Coordinates c;
	};

	struct Bus {
		std::string Name;
		std::deque<Stop*> Stops;
		bool is_round_ = false;
	};
	struct PairStops
	{
		Stop* st1;
		Stop* st2;

		bool operator==(const PairStops& other) const {
			return (st1 == other.st1 && st2 == other.st2);
		}
	};
	struct PairStopsHasher {
		size_t operator()(const PairStops& ps) const {
			std::size_t h1 = std::hash<const void*>{}(ps.st1);
			std::size_t h2 = std::hash<const void*>{}(ps.st2);
			return h1 ^ (h2 << 1);
		}
	};

	void AddStop(std::pair<std::string, Coordinates> stop);
	void AddBus(std::pair<std::pair<std::string, std::vector<std::string>>, bool> bus);

	Stop* FindStop(const std::string& name);
	Bus* FindBus(const std::string& name);
	std::string GetInfoBus(const std::string& name) const;
	std::string GetInfoStop(const std::string& name) const;
	void AddDistance(std::unordered_map<std::string, std::vector<std::pair<int, std::string>>> dist);
	int GetDistance(Stop* st1, Stop* st2) const;

	void PrintAll();
private:
	std::deque<Stop> stops_;
	std::deque<Bus> buses_;
	std::map<Stop*, std::set<Bus*>> stop_to_buses_;
	std::unordered_map<PairStops, int, PairStopsHasher> dist_;
};