//
// Created by petr on 10/30/19.
//

#include "FastChunkGen.h"
#include "geGL_utils.h"
#include "lookuptables.h"
#include "shader_literals.h"
#include <exceptions.h>
#include <experimental/array>
#include <geGL/StaticCalls.h>
#include <gl_utils.h>
#include <glm/gtc/matrix_transform.hpp>
#include <logger.h>

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
  passThrough6 = "fast/6_pass_through"_vert;
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
  vertexBuffer = createBuffer<glm::vec4>(componentCount * 5, GL_DYNAMIC_DRAW);
  normalBuffer = createBuffer<glm::vec3>(componentCount * 5, GL_DYNAMIC_DRAW);
  vertexIDsBuffer =
      createBuffer<glm::uvec3>(componentCount * 3, GL_DYNAMIC_DRAW);
  indexBuffer = createBuffer<glm::uvec3>(componentCount * 5, GL_DYNAMIC_DRAW);
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

  edgeMarkersVertexArray = std::make_shared<ge::gl::VertexArray>();
  edgeMarkersVertexArray->addAttrib(edgeBuffer, 0, 1, GL_UNSIGNED_INT,
                                    sizeof(uint), 0, GL_FALSE, 0,
                                    ge::gl::VertexArray::I);

  drawVertexArray = std::make_shared<ge::gl::VertexArray>();
  drawVertexArray->addAttrib(vertexBuffer, 0, 4, GL_FLOAT,
                                    sizeof(float) * 4, 0, GL_FALSE);
  drawVertexArray->addAttrib(normalBuffer, 1, 3, GL_FLOAT,
                                    sizeof(float) * 3, 0, GL_FALSE);
  edgeMarkersVertexArray->addElementBuffer(indexBuffer);

  ge::gl::glGenTransformFeedbacks(1, &transFeedbackName1);
  ge::gl::glGenTransformFeedbacks(1, &transFeedbackName2);
  ge::gl::glGenTransformFeedbacks(1, &transFeedbackName3);
  ge::gl::glGenTransformFeedbacks(1, &transFeedbackName4);

  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transFeedbackName1);
  ge::gl::glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0,
                           caseBuffer->getId());
  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transFeedbackName2);
  ge::gl::glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0,
                           edgeBuffer->getId());
  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transFeedbackName3);
  ge::gl::glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0,
                           vertexBuffer->getId());
  ge::gl::glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1,
                           normalBuffer->getId());
  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transFeedbackName4);
  ge::gl::glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0,
                           indexBuffer->getId());
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
  ge::gl::glAttachShader(generateIndicesProgram, passThrough6->getId());
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

void FastChunkGen::test(GLint normalProgram, GLint program, CameraController &cameraController) {
  using namespace LoggerStreamModifiers;
  Logger<true> logger;
  logger.startTime();
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
  logger << info() << "Generated primitive count(cases): " << geometryQuery.getui()
            << "\n";
  const auto caseCount = geometryQuery.getui();

  ge::gl::glUseProgram(streamEdgeMarkersProgram);
  caseMarkersVertexArray->bind();
  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transFeedbackName2);
  ge::gl::glBeginTransformFeedback(GL_POINTS);
  geometryQuery.begin();

  ge::gl::glDrawArrays(GL_POINTS, 0, geometryQuery.getui());

  geometryQuery.end();
  ge::gl::glEndTransformFeedback();
  logger << info() << "Generated primitive count(edges): " << geometryQuery.getui()
            << "\n";
  const auto edgeCount = geometryQuery.getui();


  ge::gl::glUseProgram(generateVerticesProgram);
  edgeMarkersVertexArray->bind();
  edgeToVertexLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  densityBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  ge::gl::glUniform1f(ge::gl::glGetUniformLocation(generateVerticesProgram, "step"), 0.1f);
  ge::gl::glUniform3fv(ge::gl::glGetUniformLocation(generateVerticesProgram, "start"), 1, &start[0]);
  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transFeedbackName3);
  ge::gl::glBeginTransformFeedback(GL_POINTS);
  geometryQuery.begin();

  ge::gl::glDrawArrays(GL_POINTS, 0, geometryQuery.getui());

  geometryQuery.end();
  ge::gl::glEndTransformFeedback();
  logger << info() << "Generated primitive count(vertices): " << geometryQuery.getui()
            << "\n";
  const auto vertexCount = geometryQuery.getui();

  ge::gl::glUseProgram(clearVertexIDsProgram);
  vertexIDsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  ge::gl::glUniform1f(ge::gl::glGetUniformLocation(clearVertexIDsProgram, "step"), 0.1f);
  ge::gl::glUniform3fv(ge::gl::glGetUniformLocation(clearVertexIDsProgram, "start"), 1, &start[0]);
  ge::gl::glDispatchCompute(4, 4, 4);

  ge::gl::glUseProgram(splatVerticesProgram);
  vertexIDsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  edgeMarkersVertexArray->bind();

  ge::gl::glDrawArrays(GL_POINTS, 0, edgeCount);


  ge::gl::glUseProgram(generateIndicesProgram);
  densityBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  polyCountLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  edgeLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  edgeToVertexLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 3);
  vertexIDsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 4);

  ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transFeedbackName4);
  caseMarkersVertexArray->bind();
  ge::gl::glBeginTransformFeedback(GL_POINTS);
  geometryQuery.begin();

  ge::gl::glDrawArrays(GL_POINTS, 0, caseCount);

  geometryQuery.end();
  ge::gl::glEndTransformFeedback();
  logger << info() << "Generated primitive count(indices): " << geometryQuery.getui()
         << "\n";

  ge::gl::glDisable(GL_RASTERIZER_DISCARD);
  ge::gl::glUseProgram(program);
  auto projection =
      glm::perspective(glm::radians(90.f), 1920.f / 1080, 0.1f, 100.0f);
  auto view = cameraController.getViewMatrix();
  ge::gl::glUniformMatrix4fv(
      ge::gl::glGetUniformLocation(program, "modelView"), 1, GL_FALSE,
      &view[0][0]);
  ge::gl::glUniformMatrix4fv(
      ge::gl::glGetUniformLocation(program, "projection"), 1,
      GL_FALSE, &projection[0][0]);

  glm::vec3 white{1, 1, 1};
  ge::gl::glUniform3fv(
      ge::gl::glGetUniformLocation(program, "lightColor"), 1,
      &white[0]);

  glm::vec3 lightPos = cameraController.camera.Position; // {2,5,2};
  ge::gl::glUniform3fv(
      ge::gl::glGetUniformLocation(program, "lightPos"), 1,
      &lightPos[0]);
  ge::gl::glUniform1f(
      ge::gl::glGetUniformLocation(program, "lightPower"), 40.f);
  ge::gl::glUniform3fv(
      ge::gl::glGetUniformLocation(program, "ambientColor"), 1,
      &white[0]);
  ge::gl::glUniform3fv(
      ge::gl::glGetUniformLocation(program, "diffuseColor"), 1,
      &white[0]);
  ge::gl::glUniform3fv(
      ge::gl::glGetUniformLocation(program, "specColor"), 1,
      &white[0]);
  ge::gl::glUniform1f(
      ge::gl::glGetUniformLocation(program, "shininess"), 16.f);
  glm::vec4 color {1, 1, 1, 1};
  ge::gl::glUniform4fv(ge::gl::glGetUniformLocation(program, "color"), 1,
                       &color[0]);
  //ge::gl::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  drawVertexArray->bind();
  indexBuffer->bind(GL_ELEMENT_ARRAY_BUFFER);
  ge::gl::glDrawElements(GL_TRIANGLES, geometryQuery.getui()*3, GL_UNSIGNED_INT, nullptr);

  ge::gl::glUseProgram(normalProgram);
  glm::vec4 normalColor{1, 0, 1, 0.6};
  ge::gl::glUniform4fv(
      ge::gl::glGetUniformLocation(normalProgram, "color"), 1,
      &normalColor[0]);
  auto model = glm::mat4();
  auto MVPmatrix = projection * view * model;
  ge::gl::glUniformMatrix4fv(
      ge::gl::glGetUniformLocation(normalProgram, "mvpUniform"), 1,
      GL_FALSE, &MVPmatrix[0][0]);
  normalBuffer->bind(GL_ARRAY_BUFFER);
  ge::gl::glDrawArrays(GL_POINTS, 0, vertexCount);


  logger.endTime();
  logger.printElapsedTime();
 // exit(0);
/*
  std::vector<uint> data;
  data.resize(geometryQuery.getui());
  vertexIDsBuffer->getData(data);
  data.resize(50);

  std::vector<glm::uvec3> data2;
  data2.resize(32 * 32 * 32 * 3);
  indexBuffer->getData(data2);
  data2.resize(50);*/

  //logger << debug() << "lut: " << mc::LUT::edges[102] << "\n" << flush();
  //logger << debug() << "indices: " << data2 << "\n" << flush();
  //logger << debug() << "buff: " << data << "\n" << flush();
}
