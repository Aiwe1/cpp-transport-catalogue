#pragma once

#include <vector>
#include <unordered_map>

#include <transport_catalogue.pb.h>

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

void SaveTo(std::ostream& output, RenderSettings& render_settings,
    RouterSettings& router_settings, const json::Array& base);

void Deserialize(std::istream& in, TransportCatalogue& tc,
    RenderSettings& render_settings, RouterSettings& router_settings);
