//
// Created by petr on 10/23/19.
//

#ifndef TERRAINGENERATION_GLSLSHADERLOADER_H
#define TERRAINGENERATION_GLSLSHADERLOADER_H

#include "exceptions.h"
#include <string>
#include <utility>

enum class ShaderType {
  Vertex, TesselationControl, TesselationEvaluation, Geometry, Fragment, Compute
};

namespace {
std::string shaderLocation;
}
class LocationResetter {
public:
  explicit LocationResetter(std::string oldLocation);
  LocationResetter(const LocationResetter &) = delete;
  LocationResetter(LocationResetter &&other) noexcept;
  ~LocationResetter();

private:
  bool moved = false;
  std::string oldLocation;
};


std::string_view getShaderLocation();

void setShaderLocation(std::string_view newShaderLocation);

LocationResetter setTempShaderLocation(std::string_view newShaderLocation);

class ShaderLoadingFailureException : public exc::Exception {
public:
  ShaderLoadingFailureException(
      std::string_view shaderName,
      std::string_view msg,
      std::experimental::source_location srcLoc);
};

std::string loadShaderFile(std::string_view name, ShaderType type);

#endif // TERRAINGENERATION_GLSLSHADERLOADER_H
