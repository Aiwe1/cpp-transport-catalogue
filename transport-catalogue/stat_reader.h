#pragma once

#include <iostream>
#include <string>
#include <istream>
#include <deque>
#include <unordered_map>
#include <algorithm>

#include "geo.h"
#include "transport_catalogue.h"

void PrintBus(const std::string& name, TransportCatalogue& tc);
void PrintStop(const std::string& name, TransportCatalogue& tc);

void ReadRequests(TransportCatalogue& tc);