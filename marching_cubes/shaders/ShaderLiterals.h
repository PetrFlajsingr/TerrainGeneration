//
// Created by petr on 10/27/19.
//

#ifndef TERRAINGENERATION_SHADERLITERALS_H
#define TERRAINGENERATION_SHADERLITERALS_H

#include "GlslShaderLoader.h"
#include <geGL/geGL.h>

namespace ShaderLiterals {
using ShaderPtr = std::shared_ptr<ge::gl::Shader>;

inline ShaderPtr operator""_vert(const char* name, std::size_t length) {
    return std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, loadShaderFile(name, ShaderType::Vertex));
}

inline ShaderPtr operator""_frag(const char* name, std::size_t length) {
  return std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, loadShaderFile(name, ShaderType::Fragment));
}

inline ShaderPtr operator""_geom(const char* name, std::size_t length) {
  return std::make_shared<ge::gl::Shader>(GL_GEOMETRY_SHADER, loadShaderFile(name, ShaderType::Geometry));
}

inline ShaderPtr operator""_comp(const char* name, std::size_t length) {
  return std::make_shared<ge::gl::Shader>(GL_COMPUTE_SHADER, loadShaderFile(name, ShaderType::Compute));
}

inline ShaderPtr operator""_tesc(const char* name, std::size_t length) {
  return std::make_shared<ge::gl::Shader>(GL_TESS_CONTROL_SHADER, loadShaderFile(name, ShaderType::TesselationControl));
}

inline ShaderPtr operator""_tese(const char* name, std::size_t length) {
  return std::make_shared<ge::gl::Shader>(GL_TESS_EVALUATION_SHADER, loadShaderFile(name, ShaderType::TesselationEvaluation));
}


}

#endif // TERRAINGENERATION_SHADERLITERALS_H
