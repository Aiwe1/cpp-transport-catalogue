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
std::pair<TransportCatalogue::Bus, std::vector<std::string>> SplitBus(const std::string& text);
std::vector<std::pair<int, std::string>> SplitDistance(std::string& text);

void ReadAll(TransportCatalogue& tc);