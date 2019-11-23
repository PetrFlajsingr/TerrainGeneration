//
// Created by petr on 10/30/19.
//

#include "ChunkManager.h"
#include "error_handling/exceptions.h"
#include "geGL_utils.h"
#include "lookuptables.h"
#include "shader_literals.h"
#include <experimental/array>
#include <geGL/StaticCalls.h>
#include <gl_utils.h>
#include <glm/gtc/matrix_transform.hpp>
#include <graphics/ViewFrustum.h>

using namespace ShaderLiterals;

ChunkManager::ChunkManager(
    std::shared_ptr<sdl2cpp::ui::CameraController> cameraController,
    JsonConfig<true> config)
    : cameraController(std::move(cameraController)), config(config), surr({
                                                                   config.get<float>("render", "viewDistance").value(),
                                                                   glm::uvec3{config.get<uint>("marching_cubes", "surroundingSize").value()},
                                                                   config.get<uint>("marching_cubes", "chunkPoolSize").value(),
                                                                   config.get<float>("marching_cubes", "chunkSize").value()}) {
  loadShaders();
  createPrograms();
  linkPrograms();
  createLUT();
  createBuffers();
  renderData = config.get<RenderData>("render").value();
}

void ChunkManager::loadShaders() {
  generateDensity = "fast/1_generate_density"_comp;
  streamCases = "fast/2_stream_cases"_geom;
  streamEdgeMarkers = "fast/3_stream_edge_markers"_geom;
  generateVertices = "fast/4_generate_vertices"_vert;
  clearVertexIDs = "fast/5_0_clear_vertex_ids"_comp;
  splatVertices = "fast/5_splat_vertices"_vert;
  genIndices = "fast/6_gen_indices"_geom;
  passThrough2 = "fast/2_pass_through"_vert;
  passThrough3 = "fast/3_stream_edge_markers"_vert;
  passThrough6 = "fast/6_pass_through"_vert;
  drawCube = "fast/cube"_geom;
  simpleFrag = "uniform_color"_frag;
  cubePass = "fast/cube_pass_through"_vert;
  gsNormalShader = "normal_to_line"_geom;
  vsNormalShader = "normal_to_line"_vert;
  fsShader = "uniform_color"_frag;
  fsBlinPhongShader = "blin_phong"_frag;
  vsBlinPhongShader = "blin_phong"_vert;
}

void ChunkManager::createLUT() {
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

void ChunkManager::createBuffers() {
  const auto componentCount = std::pow(32, 3);
  vertexIDsBuffer = createBuffer<uint>(componentCount * 3, GL_DYNAMIC_DRAW);
  caseBuffer = createBuffer<uint>(componentCount, GL_DYNAMIC_DRAW);
  auto chunkCoords = generateChunkCoords();
  chunkCoordBuffer = createBuffer(chunkCoords, GL_STATIC_COPY);
  edgeBuffer = createBuffer<uint>(componentCount * 3, GL_DYNAMIC_DRAW);

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

  transformFeedback1.setBuffers(caseBuffer);
  transformFeedback2.setBuffers(edgeBuffer);
}

void ChunkManager::createPrograms() {
  using namespace std::experimental;
  auto setVaryings = [](const auto &program, auto &&names,
                        GLenum bufferMode = GL_SEPARATE_ATTRIBS) {
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

  drawCubeBoundariesProgram = ge::gl::glCreateProgram();
  ge::gl::glAttachShader(drawCubeBoundariesProgram, cubePass->getId());
  ge::gl::glAttachShader(drawCubeBoundariesProgram, drawCube->getId());
  ge::gl::glAttachShader(drawCubeBoundariesProgram, simpleFrag->getId());

  drawNormalsProgram = ge::gl::glCreateProgram();
  ge::gl::glAttachShader(drawNormalsProgram, gsNormalShader->getId());
  ge::gl::glAttachShader(drawNormalsProgram, vsNormalShader->getId());
  ge::gl::glAttachShader(drawNormalsProgram, fsShader->getId());

  bpDrawProgram = ge::gl::glCreateProgram();
  ge::gl::glAttachShader(bpDrawProgram, fsBlinPhongShader->getId());
  ge::gl::glAttachShader(bpDrawProgram, vsBlinPhongShader->getId());
}

void ChunkManager::linkPrograms() {
  auto linkAndCheck = [](const auto &program) {
    ge::gl::glLinkProgram(program);
    if (!checkProgramLinkStatus(program)) {
      throw std::runtime_error("Program could not be linked");
    }
  };
  linkAndCheck(generateDensityProgram);
  linkAndCheck(streamCasesProgram);
  linkAndCheck(streamEdgeMarkersProgram);
  linkAndCheck(generateVerticesProgram);
  linkAndCheck(clearVertexIDsProgram);
  linkAndCheck(splatVerticesProgram);
  linkAndCheck(generateIndicesProgram);
  linkAndCheck(drawCubeBoundariesProgram);
  linkAndCheck(drawNormalsProgram);
  linkAndCheck(bpDrawProgram);
}

void ChunkManager::draw(DrawMode mode, DrawOptions drawOptions) {
  ge::gl::glEnable(GL_DEPTH_TEST);
  // ge::gl::glEnable(GL_CULL_FACE);
  // ge::gl::glCullFace(GL_FRONT);
  auto projection =
      glm::perspective(glm::radians(60.f), 1920.f / 1080, 0.1f, 1000.0f);
  auto view = cameraController->getViewMatrix();
  glm::vec3 lightPos =
      glm::vec3{5, 5, 5}; // cameraController.camera.Position; // {2,5,2};
  auto model = glm::mat4();
  auto MVPmatrix = projection * view * model;

  switch (mode) {
  case DrawMode::Polygon:
    ge::gl::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;
  case DrawMode::Line:
    ge::gl::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    break;
  }

  geo::ViewFrustum viewFrustum =
      geo::ViewFrustum::FromProjectionView(view, projection);

  std::vector<Chunk *> visibleChunks;
  for (auto &chunk : chunks) {
    if (renderData.viewFrustumCulling &&
        viewFrustum.contains(chunk->boundingBox) !=
        geo::FrustumPosition::Outside) {
      if (chunk->boundingSphere.distance(cameraController->camera.Position) <
          renderData.viewDistance) {
        visibleChunks.emplace_back(chunk);
      } else {
        chunk->setComputed(false);
        chunk->startPosition = cameraController->camera.Position;
        chunk->recalc();
      }
    }
  }
  static auto last = 0;
  if (last != visibleChunks.size()) {
    print("Drawing {} chunks"_s.format(visibleChunks.size()));
    last = visibleChunks.size();
  }

  drawChunk(visibleChunks, projection, view, lightPos);
  if (drawOptions.drawNormals) {
    drawNormals(visibleChunks, MVPmatrix);
  }
  if (drawOptions.drawChunkArea) {
    drawChunkCubes(visibleChunks, MVPmatrix, drawOptions.chunkAreaStep);
  }
  ge::gl::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ChunkManager::drawChunk(const std::vector<Chunk *> &chunks,
                             glm::mat4 projection, glm::mat4 modelView,
                             glm::vec3 lightPos) {
  static auto modelViewLoc =
      ge::gl::glGetUniformLocation(bpDrawProgram, "modelView");
  static auto projectionLoc =
      ge::gl::glGetUniformLocation(bpDrawProgram, "projection");
  static auto lightColorLoc =
      ge::gl::glGetUniformLocation(bpDrawProgram, "lightColor");
  static auto lightPosLoc =
      ge::gl::glGetUniformLocation(bpDrawProgram, "lightPos");
  static auto lightPowerLoc =
      ge::gl::glGetUniformLocation(bpDrawProgram, "lightPower");
  static auto ambientColorLoc =
      ge::gl::glGetUniformLocation(bpDrawProgram, "ambientColor");
  static auto diffuseColorLoc =
      ge::gl::glGetUniformLocation(bpDrawProgram, "diffuseColor");
  static auto specColorLoc =
      ge::gl::glGetUniformLocation(bpDrawProgram, "specColor");
  static auto shininessLoc =
      ge::gl::glGetUniformLocation(bpDrawProgram, "shininess");
  static auto colorLoc = ge::gl::glGetUniformLocation(bpDrawProgram, "color");
  ge::gl::glUseProgram(bpDrawProgram);
  glm::vec3 white{1, 1, 1};

  ge::gl::glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, &modelView[0][0]);
  ge::gl::glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

  ge::gl::glUniform3fv(lightColorLoc, 1, &light.color[0]);

  ge::gl::glUniform3fv(lightPosLoc, 1, &light.position[0]);
  ge::gl::glUniform1f(lightPowerLoc, light.power);
  ge::gl::glUniform3fv(ambientColorLoc, 1, &light.ambientColor[0]);
  ge::gl::glUniform3fv(diffuseColorLoc, 1, &light.diffuseColor[0]);
  ge::gl::glUniform3fv(specColorLoc, 1, &light.specColor[0]);
  ge::gl::glUniform1f(shininessLoc, material.shininess);
  ge::gl::glUniform4fv(colorLoc, 1, &material.color[0]);

  for (auto &chunk : chunks) {
    chunk->getVA()->bind();
    ge::gl::glDrawElements(GL_TRIANGLES, chunk->indexCount, GL_UNSIGNED_INT,
                           nullptr);
  }
}

void ChunkManager::drawNormals(const std::vector<Chunk *> &chunks,
                               glm::mat4 MVPmatrix) {
  ge::gl::glUseProgram(drawNormalsProgram);
  glm::vec4 normalColor{1, 0, 1, 0.6};
  ge::gl::glUniform4fv(
      ge::gl::glGetUniformLocation(drawNormalsProgram, "color"), 1,
      &normalColor[0]);

  ge::gl::glUniformMatrix4fv(
      ge::gl::glGetUniformLocation(drawNormalsProgram, "mvpUniform"), 1,
      GL_FALSE, &MVPmatrix[0][0]);
  for (auto &chunk : chunks) {
    chunk->getVA()->bind();
    ge::gl::glDrawArrays(GL_POINTS, 0, chunk->vertexCount);
  }
}

void ChunkManager::drawChunkCubes(const std::vector<Chunk *> &chunks,
                                  glm::mat4 MVPmatrix, uint step) {
  ge::gl::glUseProgram(drawCubeBoundariesProgram);
  for (auto &chunk : chunks) {
    ge::gl::glUniform1f(
        ge::gl::glGetUniformLocation(drawCubeBoundariesProgram, "step"),
        chunk->step);
    ge::gl::glUniform3fv(
        ge::gl::glGetUniformLocation(drawCubeBoundariesProgram, "start"), 1,
        &chunk->startPosition[0]);
    ge::gl::glUniformMatrix4fv(
        ge::gl::glGetUniformLocation(drawCubeBoundariesProgram, "mvpUniform"),
        1, GL_FALSE, &MVPmatrix[0][0]);

    auto colorUni =
        ge::gl::glGetUniformLocation(drawCubeBoundariesProgram, "color");
    auto cubeStep =
        ge::gl::glGetUniformLocation(drawCubeBoundariesProgram, "cubeStep");
    ge::gl::glUniform1ui(cubeStep, step);
    glm::vec4 red{1, 0, 0, 1};

    ge::gl::glLineWidth(0.05f);
    ge::gl::glUniform4fv(colorUni, 1, &red[0]);

    ge::gl::glDrawArrays(GL_POINTS, 0, 32 * 32 * 32);
  }
}

void ChunkManager::calculateDensity(const std::vector<Chunk *> &chunks) {
  static auto stepLocation =
      ge::gl::glGetUniformLocation(generateDensityProgram, "step");
  static auto startLocation =
      ge::gl::glGetUniformLocation(generateDensityProgram, "start");
  ge::gl::glUseProgram(generateDensityProgram);
  for (auto &chunk : chunks) {
    chunk->getBuffer(Chunk::Density)->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    ge::gl::glUniform1f(stepLocation, chunk->step);
    ge::gl::glUniform3fv(startLocation, 1, &chunk->startPosition[0]);
    ge::gl::glDispatchCompute(4, 4, 4);
    ge::gl::glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }
}

void ChunkManager::streamIdxVert(const std::vector<Chunk *> &chunks,
                                 ge::gl::AsynchronousQuery &query) {
  for (auto chunk : chunks) {
    ge::gl::glUseProgram(streamCasesProgram);
    chunk->getBuffer(Chunk::Density)->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    chunkCoordVertexArray->bind();
    transformFeedback1.begin(GL_POINTS);
    query.begin();
    ge::gl::glDrawArrays(GL_POINTS, 0, std::pow(chunk->size, 3));
    query.end();
    transformFeedback1.end();
    const uint caseCount = query.getui();
    if (caseCount != 0) {

      ge::gl::glUseProgram(streamEdgeMarkersProgram);
      caseMarkersVertexArray->bind();

      transformFeedback2.begin(GL_POINTS);
      query.begin();
      ge::gl::glDrawArrays(GL_POINTS, 0, query.getui());
      query.end();
      transformFeedback2.end();
      const auto edgeCount = query.getui();

      ge::gl::glUseProgram(generateVerticesProgram);
      edgeMarkersVertexArray->bind();
      edgeToVertexLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
      chunk->getBuffer(Chunk::Density)->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
      ge::gl::glUniform1f(
          ge::gl::glGetUniformLocation(generateVerticesProgram, "step"),
          chunk->step);
      ge::gl::glUniform3fv(
          ge::gl::glGetUniformLocation(generateVerticesProgram, "start"), 1,
          &chunk->startPosition[0]);

      transformFeedback3.setBuffers(chunk->getBuffer(Chunk::Vertex),
                                    chunk->getBuffer(Chunk::Normal));
      transformFeedback3.begin(GL_POINTS);
      query.begin();

      ge::gl::glDrawArrays(GL_POINTS, 0, query.getui());

      query.end();
      transformFeedback3.end();

      chunk->vertexCount = query.getui();

      ge::gl::glUseProgram(clearVertexIDsProgram);
      vertexIDsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
      ge::gl::glDispatchCompute(4, 4, 4);

      ge::gl::glUseProgram(splatVerticesProgram);
      vertexIDsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
      edgeMarkersVertexArray->bind();

      ge::gl::glDrawArrays(GL_POINTS, 0, edgeCount);

      ge::gl::glUseProgram(generateIndicesProgram);
      chunk->getBuffer(Chunk::Density)->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
      polyCountLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
      edgeLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
      edgeToVertexLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 3);
      vertexIDsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 4);

      transformFeedback4.setBuffers(chunk->getBuffer(Chunk::Index));
      transformFeedback4.begin(GL_POINTS);
      caseMarkersVertexArray->bind();
      query.begin();
      ge::gl::glDrawArrays(GL_POINTS, 0, caseCount);
      query.end();
      transformFeedback4.end();
      chunk->indexCount = query.getui() * 3;
    } else {
      chunk->vertexCount = 0;
      chunk->indexCount = 0;
    }

    chunk->setComputed(true);
    if (chunk->indexCount != 0) {
      surr.setFilled(chunk);
    } else {
      surr.setEmpty(chunk);
    }
  }
}

void ChunkManager::generateChunks() {

  std::vector<Chunk *> ptrs;
  chunks = surr.getForCompute(cameraController->camera.Position);
  for (auto chunk : chunks) {
    if (!chunk->isComputed()) {
      ptrs.emplace_back(chunk);
    }
  }
  if (ptrs.empty()) {
    return;
  }
  static auto last = 0;
  if (last != ptrs.size()) {
    print("Generating {} chunks"_s.format(ptrs.size()));
    last = ptrs.size();
  }
  ge::gl::glEnable(GL_RASTERIZER_DISCARD);
  calculateDensity(ptrs);

  ge::gl::AsynchronousQuery query{GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
                                  GL_QUERY_RESULT,
                                  ge::gl::AsynchronousQuery::UINT32};

  streamIdxVert(ptrs, query);
  ge::gl::glDisable(GL_RASTERIZER_DISCARD);
}

bool isComputed = false;
