//
// Created by petr on 10/30/19.
//

#ifndef TERRAINGENERATION_FASTCHUNKGEN_H
#define TERRAINGENERATION_FASTCHUNKGEN_H

#include "../gui/CameraController.h"
#include <geGL/geGL.h>

class FastChunkGen {
public:
  FastChunkGen();

  void test(GLint normalProgram, GLint program, CameraController &cameraController);
private:

  using Shader = std::shared_ptr<ge::gl::Shader>;
  using Program = GLuint;
  using Buffer = std::shared_ptr<ge::gl::Buffer>;
  using VertexArray = std::shared_ptr<ge::gl::VertexArray>;

  Shader generateDensity;
  Shader streamCases;
  Shader streamEdgeMarkers;
  Shader generateVertices;
  Shader clearVertexIDs;
  Shader splatVertices;
  Shader genIndices;
  Shader passThrough2;
  Shader passThrough3;
  Shader passThrough6;

  Program generateDensityProgram;
  Program streamCasesProgram;
  Program streamEdgeMarkersProgram;
  Program generateVerticesProgram;
  Program clearVertexIDsProgram;
  Program splatVerticesProgram;
  Program generateIndicesProgram;

  Buffer polyCountLUTBuffer;
  Buffer edgeLUTBuffer;
  Buffer edgeToVertexLUTBuffer;
  Buffer densityBuffer;
  Buffer vertexBuffer;
  Buffer normalBuffer;
  Buffer vertexIDsBuffer;
  Buffer indexBuffer;
  Buffer chunkCoordBuffer;
  Buffer caseBuffer;
  Buffer edgeBuffer;

  VertexArray chunkCoordVertexArray;
  VertexArray caseMarkersVertexArray;
  VertexArray edgeMarkersVertexArray;
  VertexArray drawVertexArray;

  GLuint transFeedbackName1;
  GLuint transFeedbackName2;
  GLuint transFeedbackName3;
  GLuint transFeedbackName4;

  void loadShaders();
  void createLUT();
  void createBuffers();
  void createPrograms();
  void linkPrograms();
};

#endif // TERRAINGENERATION_FASTCHUNKGEN_H
