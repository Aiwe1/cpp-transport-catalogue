#pragma once

#include <vector>
#include <unordered_map>

#include <transport_catalogue.pb.h>

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

transport_catalogue_serialize::Color SaveColor(const svg::Color& col);

void SaveToRender(transport_catalogue_serialize::TransportCatalogue& tcs, RenderSettings& render_settings);

void SaveToTC(transport_catalogue_serialize::TransportCatalogue& tcs, const json::Array& base);

void SaveTo(std::ostream& output, RenderSettings& render_settings,
    RouterSettings& router_settings, const json::Array& base);

svg::Color DeserializeColor(const transport_catalogue_serialize::Color& c);

void DeserializeRender(RenderSettings& render_settings, const transport_catalogue_serialize::RenderSettings& rs);

void DeserializeTC(TransportCatalogue& tc, const transport_catalogue_serialize::TransportCatalogue& tcs);

void Deserialize(std::istream& in, TransportCatalogue& tc,
    RenderSettings& render_settings, RouterSettings& router_settings);
