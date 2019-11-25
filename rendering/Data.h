//
// Created by petr on 11/18/19.
//

#ifndef TERRAINGENERATION_DATA_H
#define TERRAINGENERATION_DATA_H

#include <nlohmann/json.hpp>

namespace {
using json = nlohmann::json;
}
struct GLData {
  bool depthTest;
  bool msaa;
  bool backfaceCulling;
};

void to_json(json &j, const GLData &data);

void from_json(const json &j, GLData &data);

struct DeviceData {
  struct {
    uint width;
    uint height;
  } screen;
};

void to_json(json &j, const DeviceData &data);

void from_json(const json &j, DeviceData &data);

struct DebugData {
  bool drawNormals;
  struct {
    bool enabled;
    uint step;
  } drawChunkBorder;
};

struct RenderData {
  float viewDistance;
  uint levelOfDetail;
  bool viewFrustumCulling;
};

void to_json(json &j, const RenderData &data);

void from_json(const json &j, RenderData &data);

#endif // TERRAINGENERATION_DATA_H
