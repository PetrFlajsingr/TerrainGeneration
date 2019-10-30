//
// Created by petr on 10/27/19.
//

#ifndef TERRAINGENERATION_SHADERLITERALS_H
#define TERRAINGENERATION_SHADERLITERALS_H

#include "GlslShaderLoader.h"

namespace ShaderLiterals {
inline std::string operator""_vert(const char* name, std::size_t length) {
    return loadShaderFile(name, ShaderType::Vertex);
}

inline std::string operator""_frag(const char* name, std::size_t length) {
  return loadShaderFile(name, ShaderType::Fragment);
}

inline std::string operator""_geom(const char* name, std::size_t length) {
  return loadShaderFile(name, ShaderType::Geometry);
}

inline std::string operator""_comp(const char* name, std::size_t length) {
  return loadShaderFile(name, ShaderType::Compute);
}

inline std::string operator""_tesc(const char* name, std::size_t length) {
  return loadShaderFile(name, ShaderType::TesselationControl);
}

inline std::string operator""_tese(const char* name, std::size_t length) {
  return loadShaderFile(name, ShaderType::TesselationEvaluation);
}


}

#endif // TERRAINGENERATION_SHADERLITERALS_H
