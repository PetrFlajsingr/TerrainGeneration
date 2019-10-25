//
// Created by petr on 10/23/19.
//

#include "GlslShaderLoader.h"
#include <fstream>

#define SHADER_FOLDER std::string("/home/petr/CLionProjects/TerrainGeneration/marching_cubes/shaders")

using namespace std::string_literals;
std::string loadShaderFile(std::string_view name, ShaderType type) {
  std::string extension;
  switch (type) {
  case ShaderType::Vertex:
    extension = ".vert";
    break;
  case ShaderType::TesselationControl:
    extension = ".tesc";
    break;
  case ShaderType::TesselationEvaluation:
    extension = ".tese";
    break;
  case ShaderType::Geometry:
    extension = ".geom";
    break;
  case ShaderType::Fragment:
    extension = ".frag";
    break;
  case ShaderType::Compute:
    extension = ".comp";
    break;
  }
  const auto pathToFile = SHADER_FOLDER + "/" + std::string(name) + extension;
  std::ifstream inStream(pathToFile);
  if (!inStream.is_open()) {
    throw "Shader opening failed: "s + std::string(name);
  }
  return std::string(std::istreambuf_iterator<char>(inStream), std::istreambuf_iterator<char>());
}

std::string operator""_shader_file(const char *fileName, size_t size) {
  std::string_view fName(fileName);
  if (auto pos = fName.find('.'); pos != std::string_view::npos) {
    return loadShaderFile(fName.substr(0, pos), extensionToShaderType(fName.substr(pos + 1)));
  }
  throw "Invalid shader file";
}
ShaderType extensionToShaderType(std::string_view extension) {
  if (extension == "vert") {
    return ShaderType::Vertex;
  }
  if (extension == "tesc") {
    return ShaderType::TesselationControl;
  }
  if (extension == "tese") {
    return ShaderType::TesselationEvaluation;
  }
  if (extension == "geom") {
    return ShaderType::Geometry;
  }
  if (extension == "frag") {
    return ShaderType::Fragment;
  }
  if (extension == "comp") {
    return ShaderType::Compute;
  }
  throw "invalid extension";
}
