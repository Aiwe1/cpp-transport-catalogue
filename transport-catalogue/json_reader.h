#pragma once

#include <iostream>
#include <string>
#include <istream>
#include <deque>
#include <unordered_map>
#include <algorithm>

#include "geo.h"
#include "transport_catalogue.h"
#include "json.h"
#include "map_renderer.h"
#include "json_builder.h"
#include "transport_router.h"

void PutBusToJson(const std::string& name, TransportCatalogue& tc, json::Builder& b);
void PutStopToJson(const std::string& name, TransportCatalogue& tc, json::Builder& b);
void PutRouteToJson(RouterSettings& router_settings, TransportCatalogue& tc, json::Builder& b);

void PrintJson(RenderSettings& rs, RouterSettings& router_settings, TransportCatalogue& tc, json::Dict& a, std::ostream& os);
void AddBusesStops(TransportCatalogue& tc, json::Array& base);
void ReadAll(TransportCatalogue& tc, std::istream& is, std::ostream& os);