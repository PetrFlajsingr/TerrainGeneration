//
// Created by petr on 10/30/19.
//

#include "FastChunkGen.h"
#include "ShaderLiterals.h"
#include "geGL_utils.h"
#include "lookuptables.h"
#include <geGL/StaticCalls.h>
#include <gl_utils.h>
#include <experimental/array>

using namespace ShaderLiterals;

FastChunkGen::FastChunkGen() {
  loadShaders();
  createPrograms();
  linkPrograms();
  createLUT();
  createBuffers();
}

void FastChunkGen::loadShaders() {
  generateDensity = "fast/1_generate_density"_comp;
  streamCases = "fast/2_stream_cases"_geom;
  streamEdgeMarkers = "fast/3_stream_edge_markers"_geom;
  generateVertices = "fast/4_generate_vertices"_vert;
  clearVertexIDs = "fast/5_0_clear_vertex_ids"_comp;
  splatVertices = "fast/5_splat_vertices"_vert;
  genIndices = "fast/6_gen_indices"_geom;
  passThrough2 = "fast/2_pass_through"_vert;
  passThrough3 = "fast/3_pass_through"_vert;
}

void FastChunkGen::createLUT() {
  polyCountLUTBuffer =
      createBuffer<decltype(mc::LUT::polygonCount)::value_type>(
          mc::LUT::polygonCount.size(), GL_STATIC_COPY,
          mc::LUT::polygonCount.data());
  edgeLUTBuffer = createBuffer<decltype(mc::LUT::edges)::value_type>(
      mc::LUT::edges.size(), GL_STATIC_COPY, mc::LUT::edges.data());
  edgeToVertexLUTBuffer =
      createBuffer<decltype(mc::LUT::edgeToVertexIds)::value_type>(
          mc::LUT::edgeToVertexIds.size(), GL_STATIC_COPY,
          mc::LUT::edgeToVertexIds.data());
}

std::vector<glm::uvec3> generateChunkCoords() {
  std::vector<glm::uvec3> result;
  result.resize(32 * 32 * 32);

  for (int x = 0; x < 32; ++x) {
    for (int y = 0; y < 32; ++y) {
      for (int z = 0; z < 32; ++z) {
        result[x + y * 32 + z * 32 * 32] = {x, y, z};
      }
    }
  }

  return result;
}

void FastChunkGen::createBuffers() {
  const auto componentCount = std::pow(32, 3);
  densityBuffer = createBuffer<float>(componentCount, GL_DYNAMIC_DRAW);
  vertexBuffer = createBuffer<glm::vec3>(componentCount * 15, GL_DYNAMIC_DRAW);
  vertexIDsBuffer =
      createBuffer<glm::uvec3>(componentCount * 3, GL_DYNAMIC_DRAW);
  indexBuffer = createBuffer<glm::uvec3>(componentCount * 15, GL_DYNAMIC_DRAW);
  caseBuffer = createBuffer<uint>(componentCount, GL_DYNAMIC_DRAW);
  auto chunkCoords = generateChunkCoords();
  //chunkCoordBuffer = createBuffer<uint>(componentCount * 3, GL_STATIC_COPY,
  //                                      chunkCoords.data());
  chunkCoordBuffer = createBuffer(chunkCoords, GL_STATIC_COPY);
  edgeBuffer =
      createBuffer<uint>(componentCount, GL_DYNAMIC_DRAW);

  chunkCoordVertexArray = std::make_shared<ge::gl::VertexArray>();
  chunkCoordVertexArray->addAttrib(chunkCoordBuffer, 0, 1, GL_UNSIGNED_INT,
                                   sizeof(uint), 0, GL_FALSE, 0,
                                   ge::gl::VertexArray::I);

  caseMarkersVertexArray = std::make_shared<ge::gl::VertexArray>();
  caseMarkersVertexArray->addAttrib(caseBuffer, 0, 1, GL_UNSIGNED_INT,
                                    sizeof(uint), 0, GL_FALSE, 0,
                                    ge::gl::VertexArray::I);

  ge::gl::glGenTransformFeedbacks(1, &transFeedbackName1);
  ge::gl::glGenTransformFeedbacks(1, &transFeedbackName2);

  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transFeedbackName1);
  ge::gl::glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0,
                           caseBuffer->getId());
  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transFeedbackName2);
  ge::gl::glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0,
                           edgeBuffer->getId());
  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

void FastChunkGen::createPrograms()
{
  using namespace std::experimental;
  auto setVaryings = [](const auto &program, auto &&names, GLenum bufferMode = GL_SEPARATE_ATTRIBS) {
    ge::gl::glTransformFeedbackVaryings(program, names.size(), names.data(),
                                        bufferMode);
  };

  generateDensityProgram = ge::gl::glCreateProgram();
  ge::gl::glAttachShader(generateDensityProgram, generateDensity->getId());

  streamCasesProgram = ge::gl::glCreateProgram();
  ge::gl::glAttachShader(streamCasesProgram, passThrough2->getId());
  ge::gl::glAttachShader(streamCasesProgram, streamCases->getId());
  setVaryings(streamCasesProgram, make_array("marker"));

  streamEdgeMarkersProgram = ge::gl::glCreateProgram();
  ge::gl::glAttachShader(streamEdgeMarkersProgram, streamEdgeMarkers->getId());
  ge::gl::glAttachShader(streamEdgeMarkersProgram, passThrough3->getId());
  setVaryings(streamEdgeMarkersProgram, make_array("edgeMarker"));

  generateVerticesProgram = ge::gl::glCreateProgram();
  ge::gl::glAttachShader(generateVerticesProgram, generateVertices->getId());
  setVaryings(generateVerticesProgram, make_array("Position", "Normal"));

  clearVertexIDsProgram = ge::gl::glCreateProgram();
  ge::gl::glAttachShader(clearVertexIDsProgram, clearVertexIDs->getId());

  splatVerticesProgram = ge::gl::glCreateProgram();
  ge::gl::glAttachShader(splatVerticesProgram, splatVertices->getId());

  generateIndicesProgram = ge::gl::glCreateProgram();
  ge::gl::glAttachShader(generateIndicesProgram, genIndices->getId());
  setVaryings(generateIndicesProgram, make_array("indices"));

  ge::gl::glProgramParameteri(generateDensityProgram, GL_PROGRAM_SEPARABLE,
                              GL_TRUE);
  ge::gl::glProgramParameteri(streamCasesProgram, GL_PROGRAM_SEPARABLE,
                              GL_TRUE);
  ge::gl::glProgramParameteri(streamEdgeMarkersProgram, GL_PROGRAM_SEPARABLE,
                              GL_TRUE);
  ge::gl::glProgramParameteri(generateVerticesProgram, GL_PROGRAM_SEPARABLE,
                              GL_TRUE);
  ge::gl::glProgramParameteri(clearVertexIDsProgram, GL_PROGRAM_SEPARABLE,
                              GL_TRUE);
  ge::gl::glProgramParameteri(splatVerticesProgram, GL_PROGRAM_SEPARABLE,
                              GL_TRUE);
  ge::gl::glProgramParameteri(generateIndicesProgram, GL_PROGRAM_SEPARABLE,
                              GL_TRUE);
}

void FastChunkGen::linkPrograms() {
  auto linkAndCheck = [](const auto &program) {
    ge::gl::glLinkProgram(program);
    if (!checkProgramLinkStatus(program)) {
      throw std::runtime_error(
          "Program could not be linked: generateDensityProgram");
    }
  };
  linkAndCheck(generateDensityProgram);
  linkAndCheck(streamCasesProgram);
  linkAndCheck(streamEdgeMarkersProgram);
  linkAndCheck(generateVerticesProgram);
  linkAndCheck(clearVertexIDsProgram);
  linkAndCheck(splatVerticesProgram);
  linkAndCheck(generateIndicesProgram);
}

void FastChunkGen::test() {
  densityBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  ge::gl::glUseProgram(generateDensityProgram);
  ge::gl::glUniform1f(
      ge::gl::glGetUniformLocation(generateDensityProgram, "step"), 1.f);
  glm::vec3 start{0, 0, 0};
  ge::gl::glUniform3fv(
      ge::gl::glGetUniformLocation(generateDensityProgram, "start"), 0,
      &start[0]);
  ge::gl::glDispatchCompute(4, 4, 4);
  ge::gl::glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  ge::gl::glUseProgram(streamCasesProgram);
  densityBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  ge::gl::glEnable(GL_RASTERIZER_DISCARD);
  ge::gl::AsynchronousQuery geometryQuery{
      GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, GL_QUERY_RESULT,
      ge::gl::AsynchronousQuery::UINT32};

  chunkCoordVertexArray->bind();
  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transFeedbackName1);
  ge::gl::glBeginTransformFeedback(GL_POINTS);
  geometryQuery.begin();

  ge::gl::glDrawArrays(GL_POINTS, 0, 32 * 32 * 32);

  geometryQuery.end();
  ge::gl::glEndTransformFeedback();
  std::cout << "Generated primitive count: " << geometryQuery.getui()
            << std::endl;

  ge::gl::glUseProgram(streamEdgeMarkersProgram);
  caseMarkersVertexArray->bind();
  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transFeedbackName2);
  ge::gl::glBeginTransformFeedback(GL_POINTS);
  geometryQuery.begin();

  ge::gl::glDrawArrays(GL_POINTS, 0, geometryQuery.getui());

  geometryQuery.end();
  ge::gl::glEndTransformFeedback();
  std::cout << "Generated primitive count: " << geometryQuery.getui()
            << std::endl;

  ge::gl::glDisable(GL_RASTERIZER_DISCARD);

  std::vector<uint> data;
  data.resize(geometryQuery.getui());
  caseBuffer->getData(data);

  std::vector<uint> data2;
  data2.resize(32 * 32 * 32 * 3);
  edgeBuffer->getData(data2);

  std::cout << "Done";
}
