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


}

#endif // TERRAINGENERATION_SHADERLITERALS_H
