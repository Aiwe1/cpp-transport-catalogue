#pragma once

#include <iostream>
#include <string>
#include <istream>
#include <deque>
#include <unordered_map>
#include <algorithm>

#include "geo.h"
#include "transport_catalogue.h"

std::string DeleteSpace(std::string s);
std::pair<TransportCatalogue::Stop, std::string> SplitStop(const std::string& text);
TransportCatalogue::BusToStops SplitBus(const std::string& text);
std::vector<std::pair<int, std::string>> SplitDistance(std::string& text);

void ReadAll(TransportCatalogue& tc, std::istream& is);
