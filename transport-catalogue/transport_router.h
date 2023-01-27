#pragma once

#include "router.h"
#include "json.h"

struct RouterSettings {
	constexpr static double METER_PER_MIN = 1000.0 / 60.0;
	explicit RouterSettings(const json::Dict& d) {
		using namespace json;

		bus_wait_time = static_cast<double>(d.at("bus_wait_time").AsInt());
		bus_velocity = d.at("bus_velocity").AsDouble() * METER_PER_MIN;
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
