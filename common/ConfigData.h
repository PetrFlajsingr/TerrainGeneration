//
// Created by petr on 1/3/20.
//

#ifndef TERRAINGENERATION_CONFIGDATA_H
#define TERRAINGENERATION_CONFIGDATA_H

#include <string>
#include <nlohmann/json.hpp>
#include "utils/error_handling/exceptions.h"

struct ConfigData {
  struct {
    bool depthTest;
    bool msaa;
    bool backfaceCulling;
  } gl;

  struct {
    std::string shaderLocation;
    std::string assetsLocation;
  } paths;

  struct {
    float viewDistance;
    unsigned int levelOfDetail;
    bool shadows;
  } render;

  struct {
    float chunkSize;
    unsigned int surroundingSize;
    unsigned int chunkPoolSize;
    bool aggressiveChunkUnloading;
    unsigned int computeBatchSize;
  } marchingCubes;
};

using json = nlohmann::json;

inline void from_json(const json &j, ConfigData &data) {
  using namespace std::string_literals;
  try {
    {
      const auto jGl = j.at("gl");
      data.gl.depthTest = jGl.at("depthTest");
      data.gl.msaa = jGl.at("msaa");
      data.gl.backfaceCulling = jGl.at("backfaceCulling");
    }
    {
      const auto jPaths = j.at("paths");
      data.paths.shaderLocation = jPaths.at("shaderLocation");
      data.paths.assetsLocation = jPaths.at("assetsLocation");
    }
    {
      const auto jRender = j.at("render");
      data.render.viewDistance = jRender.at("viewDistance");
      data.render.levelOfDetail = jRender.at("levelOfDetail");
      data.render.shadows = jRender.at("shadows");
    }
    {
      const auto jMarchingCubes = j.at("marching_cubes");
      data.marchingCubes.chunkSize = jMarchingCubes.at("chunkSize");
      data.marchingCubes.surroundingSize = jMarchingCubes.at("surroundingSize");
      data.marchingCubes.chunkPoolSize = jMarchingCubes.at("chunkPoolSize");
      data.marchingCubes.aggressiveChunkUnloading = jMarchingCubes.at("aggressiveChunkUnloading");
      data.marchingCubes.computeBatchSize = jMarchingCubes.at("computeBatchSize");
    }
  } catch (const std::exception &e) {
    throw exc::SerialisationException("Could not de-serialize ConfigData:\n"s + e.what());
  }
}

#endif // TERRAINGENERATION_CONFIGDATA_H
