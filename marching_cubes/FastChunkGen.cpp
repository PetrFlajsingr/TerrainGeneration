//
// Created by petr on 10/30/19.
//

#include "FastChunkGen.h"
#include "shaders/ShaderLiterals.h"

using namespace ShaderLiterals;

void FastChunkGen::loadShaders() {
  generateDensity = "fast/generate_density"_comp;
  streamCases = "fast/generate_density"_geom;
  streamEdgeMarkers = "fast/generate_density"_geom;
  generateVertices = "fast/generate_density"_vert;
  clearVertexIDs = "fast/generate_density"_comp;
  splatVertices = "fast/generate_density"_vert;
  genIndices = "fast/generate_density"_geom;
}
