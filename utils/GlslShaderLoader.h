//
// Created by petr on 10/23/19.
//

#ifndef TERRAINGENERATION_GLSLSHADERLOADER_H
#define TERRAINGENERATION_GLSLSHADERLOADER_H

#include <string>

enum class ShaderType {
  Vertex, TesselationControl, TesselationEvaluation, Geometry, Fragment, Compute
};

ShaderType extensionToShaderType(std::string_view extension);

std::string loadShaderFile(std::string_view name, ShaderType type);

std::string operator"" _shader_file(const char* fileName, size_t size);

#endif // TERRAINGENERATION_GLSLSHADERLOADER_H
