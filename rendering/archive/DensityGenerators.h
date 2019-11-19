//
// Created by petr on 10/20/19.
//

#ifndef TERRAINGENERATION_DENSITYGENERATORS_H
#define TERRAINGENERATION_DENSITYGENERATORS_H
/*
#include "../../gui/CameraController.h"
#include "../ChunkManager.h"
#include "../lookuptables.h"
#include "GlslShaderLoader.h"
#include "OldChunk.h"
#include "shader_literals.h"
#include <chrono>
#include <functional>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <gl_utils.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <logger.h>
#include <now.h>
#include <optional>
#include <string>
using namespace ShaderLiterals;
using namespace LoggerStreamModifiers;

#define DRAW_MESH0
#define DRAW_NORMALS0
#define DRAW_SKELETON0
static bool drawAllSkeletons = false;

using namespace std::chrono_literals;
constexpr uint skeletonStep = 31;
struct Compute {
  const float d = 1 / 1.f;
  const float w = 1 / (d * 32.0);
  std::vector<mc::OldChunk> chunks;

  glm::vec3 gravityCenter{0, -3, 0};

  decltype(Logger{std::cout}) logger = Logger{std::cout};

  glm::mat4 projection =
      glm::perspective(glm::radians(45.f), 1920.f / 1080, 0.1f, 100.0f);

  void generateChunks() {
    const uint chunkCount = 10;
    const float step = 1;
    const glm::vec3 offset{-4, -5, -2.5};
    // const glm::vec3 offset{-22, -30, -20};
    for (float i = 0; i < chunkCount * step; i += step) {
      for (float j = 0; j < chunkCount * step; j += step) {
        for (float k = 0; k < chunkCount * step; k += step) {
          chunks.emplace_back(
              32, 1.0f * step,
              glm::vec3{i / d - i * w, j / d - j * w, k / d - k * w} + offset,
              glm::vec4{.2, 0.2, 0.2, 0} +
                  glm::vec4(i / static_cast<float>(chunkCount * step),
                            j / static_cast<float>(chunkCount * step),
                            k / static_cast<float>(chunkCount * step), 1));
        }
      }
    }
  }

  uint cnt2 = 0;

  std::chrono::milliseconds total = 0s;
  std::chrono::milliseconds TFtime = 0s;
  std::chrono::milliseconds CStime = 0s;
  std::chrono::milliseconds rendertime = 0s;
  int cntComp = 0, cntRender = 0;

  Compute() {
    //generateChunks();
    csShader = "mc_chunk"_comp;
    // csProgram = std::make_shared<ge::gl::Program>();
    csProgram = ge::gl::glCreateProgram();
    ge::gl::glAttachShader(csProgram, csShader->getId());

    gsShader = std::make_shared<ge::gl::Shader>(
        GL_GEOMETRY_SHADER,
        loadShaderFile("cubes_to_triangles", ShaderType::Geometry));
    gsCubeShader = std::make_shared<ge::gl::Shader>(
        GL_GEOMETRY_SHADER, loadShaderFile("cube_edges", ShaderType::Geometry));
    vsShaderLight = std::make_shared<ge::gl::Shader>(
        GL_VERTEX_SHADER, loadShaderFile("mvp_light", ShaderType::Vertex));
    vsShader = std::make_shared<ge::gl::Shader>(
        GL_VERTEX_SHADER, loadShaderFile("mvp", ShaderType::Vertex));
    fsShaderLight = std::make_shared<ge::gl::Shader>(
        GL_FRAGMENT_SHADER,
        loadShaderFile("uniform_color_light", ShaderType::Fragment));
    fsShader = std::make_shared<ge::gl::Shader>(
        GL_FRAGMENT_SHADER,
        loadShaderFile("uniform_color", ShaderType::Fragment));

    fsBlinPhongShader = "blin_phong"_frag;
    vsBlinPhongShader = "blin_phong"_vert;

    bpDrawProgram = ge::gl::glCreateProgram();
    ge::gl::glAttachShader(bpDrawProgram, fsBlinPhongShader->getId());
    ge::gl::glAttachShader(bpDrawProgram, vsBlinPhongShader->getId());
    ge::gl::glLinkProgram(bpDrawProgram);
    if (!checkProgramLinkStatus(bpDrawProgram)) {
      throw std::runtime_error(
          "Program could not be linked: generateDensityProgram");
    }

    drawNormalsProgram = ge::gl::glCreateProgram();

    gsNormalShader = "normal_to_line"_geom;
    vsNormalShader = "normal_to_line"_vert;
    ge::gl::glAttachShader(drawNormalsProgram, gsNormalShader->getId());
    ge::gl::glAttachShader(drawNormalsProgram, vsNormalShader->getId());
    ge::gl::glAttachShader(drawNormalsProgram, fsShader->getId());
    ge::gl::glLinkProgram(drawNormalsProgram);

    if (!checkProgramLinkStatus(drawNormalsProgram)) {
      throw std::runtime_error(
          "Program could not be linked: generateDensityProgram");
    }

    gsProgram = ge::gl::glCreateProgram();
    // ge::gl::glAttachShader(gsProgram, vsShader->getId());
    ge::gl::glAttachShader(gsProgram, gsShader->getId());

    drawProgram = ge::gl::glCreateProgram();
    ge::gl::glAttachShader(drawProgram, vsShaderLight->getId());
    ge::gl::glAttachShader(drawProgram, fsShaderLight->getId());

    chunkSkeletonDrawProgram = ge::gl::glCreateProgram();
    ge::gl::glAttachShader(chunkSkeletonDrawProgram, vsShader->getId());
    ge::gl::glAttachShader(chunkSkeletonDrawProgram, gsCubeShader->getId());
    ge::gl::glAttachShader(chunkSkeletonDrawProgram, fsShader->getId());

    ge::gl::glProgramParameteri(csProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);
    ge::gl::glProgramParameteri(gsProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);
    ge::gl::glProgramParameteri(drawProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);

    ge::gl::glLinkProgram(csProgram);
    ge::gl::glLinkProgram(drawProgram);
    ge::gl::glLinkProgram(chunkSkeletonDrawProgram);
    if (!checkProgramLinkStatus(chunkSkeletonDrawProgram)) {
      throw std::runtime_error(
          "Program could not be linked: generateDensityProgram");
    }

    const GLchar *varyings[] = {"Wat.pos", "Wat.normal"};
    ge::gl::glTransformFeedbackVaryings(gsProgram, 2, varyings,
                                        GL_SEPARATE_ATTRIBS);
    ge::gl::glLinkProgram(gsProgram);

    if (!checkProgramLinkStatus(gsProgram)) {
      throw std::runtime_error(
          "Program could not be linked: generateDensityProgram");
    }

    polyCountLUTBuffer = std::make_shared<ge::gl::Buffer>(
        mc::LUT::polygonCount.size() * sizeof(uint32_t),
        mc::LUT::polygonCount.data(), GL_STATIC_COPY);
    edgeLUTBuffer = std::make_shared<ge::gl::Buffer>(
        mc::LUT::edges.size() * sizeof(uint32_t) * 3 * 5, mc::LUT::edges.data(),
        GL_STATIC_COPY);
    edgeToVertexLUTBuffer = std::make_shared<ge::gl::Buffer>(
        mc::LUT::edgeToVertexIds.size() * sizeof(uint32_t) * 2,
        mc::LUT::edgeToVertexIds.data(), GL_STATIC_COPY);

    cameraController = CameraController(glm::vec3{.10f, .10f, 10.f});
  }

  void operator()() {
    auto startTime = now<std::chrono::milliseconds>();
    ge::gl::glEnable(GL_MULTISAMPLE);
    ge::gl::glEnable(GL_DEPTH_TEST);
    ge::gl::glCullFace(GL_FRONT);
    ge::gl::glEnable(GL_CULL_FACE);
    static float offset = 0;
    offset += 0.02;
    if (std::any_of(chunks.begin(), chunks.end(),
                    [](const auto &a) { return !a.isComputed(); })) {

      logger << "Running compute\n";
      ge::gl::glUseProgram(csProgram);

      auto uni = ge::gl::glGetUniformLocation(csProgram, "bah");
      ge::gl::glUniform1f(ge::gl::glGetUniformLocation(csProgram, "offset"),
                          offset);
      glm::vec3 start{0, 0, 0};
      ge::gl::glUniform3fv(ge::gl::glGetUniformLocation(csProgram, "start"), 1,
                           &start[0]);

      ge::gl::glUniform1ui(uni, 0);

      for (auto &chunk : chunks) {
        if (!chunk.isComputed()) {
          chunk.dispatchDensityComputation(csProgram, Blocking::No);
        }
      }

      ge::gl::glUniform1ui(uni, 1);

      for (auto &chunk : chunks) {
        if (!chunk.isComputed()) {
          chunk.dispatchCubeIndicesComputation(csProgram, Blocking::No);
        }
      }

      CStime += now<std::chrono::milliseconds>() - startTime;
      startTime = now<std::chrono::milliseconds>();

      ge::gl::glUseProgram(gsProgram);
      ge::gl::glEnable(GL_RASTERIZER_DISCARD);

      polyCountLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
      edgeLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
      edgeToVertexLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 3);

      for (auto &chunk : chunks) {
        if (!chunk.isComputed()) {
          chunk.calculateVertices(gsProgram);
        }
      }
      ge::gl::glDisable(GL_RASTERIZER_DISCARD);

      ++cntComp;
      TFtime += now<std::chrono::milliseconds>() - startTime;
    }
    startTime = now<std::chrono::milliseconds>();

    glm::vec3 up =
        glm::normalize((gravityCenter - cameraController.camera.Position));
    auto view = cameraController.getViewMatrix();
    auto model = glm::mat4();
    auto MVPmatrix = projection * view * model;

    glm::vec4 red{1, 0, 0, 1};

    geo::ViewFrustum viewFrustum =
        geo::ViewFrustum::FromProjectionView(view, projection);


#ifdef DRAW_SKELETON
    ge::gl::glUseProgram(chunkSkeletonDrawProgram);

    auto colorUni =
        ge::gl::glGetUniformLocation(chunkSkeletonDrawProgram, "color");
    ge::gl::glUniformMatrix4fv(
        ge::gl::glGetUniformLocation(chunkSkeletonDrawProgram, "mvpUniform"), 1,
        GL_FALSE, &MVPmatrix[0][0]);

    ge::gl::glLineWidth(0.01f);
    ge::gl::glUniform4fv(colorUni, 1, &red[0]);

    ge::gl::glUniform1ui(
        ge::gl::glGetUniformLocation(chunkSkeletonDrawProgram, "step"),
        skeletonStep);
    for (auto &chunk : chunks) {
      if (drawAllSkeletons || chunk.shouldBeDrawn(viewFrustum)) {
        chunk.vertexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
        ge::gl::glDrawArrays(GL_POINTS, 0, chunk.componentCount);
      }
    }
#endif
    if (std::any_of(chunks.begin(), chunks.end(), [&viewFrustum](auto &chunk) {
          return chunk.shouldBeDrawn();
        })) {
      ge::gl::glUseProgram(bpDrawProgram);


      ge::gl::glUniformMatrix4fv(
          ge::gl::glGetUniformLocation(bpDrawProgram, "modelView"), 1, GL_FALSE,
          &view[0][0]);
      ge::gl::glUniformMatrix4fv(
          ge::gl::glGetUniformLocation(bpDrawProgram, "projection"), 1,
          GL_FALSE, &projection[0][0]);

      glm::vec3 white{1, 1, 1};
      ge::gl::glUniform3fv(
          ge::gl::glGetUniformLocation(bpDrawProgram, "lightColor"), 1,
          &white[0]);

      glm::vec3 lightPos = cameraController.camera.Position; // {2,5,2};
      ge::gl::glUniform3fv(
          ge::gl::glGetUniformLocation(bpDrawProgram, "lightPos"), 1,
          &lightPos[0]);
      ge::gl::glUniform1f(
          ge::gl::glGetUniformLocation(bpDrawProgram, "lightPower"), 40.f);
      ge::gl::glUniform3fv(
          ge::gl::glGetUniformLocation(bpDrawProgram, "ambientColor"), 1,
          &white[0]);
      ge::gl::glUniform3fv(
          ge::gl::glGetUniformLocation(bpDrawProgram, "diffuseColor"), 1,
          &white[0]);
      ge::gl::glUniform3fv(
          ge::gl::glGetUniformLocation(bpDrawProgram, "specColor"), 1,
          &white[0]);
      ge::gl::glUniform1f(
          ge::gl::glGetUniformLocation(bpDrawProgram, "shininess"), 16.f);


      int cnt = 0;
#ifdef DRAW_MESH
      ge::gl::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      for (auto &chunk : chunks) {
        if (chunk.shouldBeDrawn(viewFrustum)) {
          ++cnt;
          chunk.render(mc::Chunk::MeshLines, drawProgram);
        }
      }
      ge::gl::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#else

      for (auto &chunk : chunks) {
        if (chunk.shouldBeDrawn(viewFrustum)) {
          ++cnt;
          chunk.render(mc::OldChunk::Mesh, bpDrawProgram);
        }
      }
#endif
      ++cntRender;
      rendertime += now<std::chrono::milliseconds>() - startTime;
    }

#ifdef DRAW_NORMALS
    ge::gl::glUseProgram(drawNormalsProgram);

    glm::vec4 normalColor{1, 0, 1, 0.6};
    ge::gl::glUniform4fv(
        ge::gl::glGetUniformLocation(drawNormalsProgram, "color"), 1,
        &normalColor[0]);

    ge::gl::glUniformMatrix4fv(
        ge::gl::glGetUniformLocation(drawNormalsProgram, "mvpUniform"), 1,
        GL_FALSE, &MVPmatrix[0][0]);

    for (auto &chunk : chunks) {
      if (chunk.shouldBeDrawn(viewFrustum)) {
        chunk.render(mc::Chunk::Normals, drawNormalsProgram);
        // ge::gl::glDrawTransformFeedback(GL_POINTS, chunk.feedbackName);
      }
    }
#endif

    if (cnt2 % 100 == 0) {
      for (auto &chunk : chunks) {
        // chunk.invalidate();
      }
      logger << debug() << "Times:=> CS:  " << CStime.count()
             << ", TF: " << TFtime.count() << ", Render: " << rendertime.count()
             << " ms.\n";
    }
    ++cnt2;
    ge::gl::glFlush();
  }
  GLuint csProgram;
  GLuint gsProgram;
  GLuint drawProgram;
  GLuint chunkSkeletonDrawProgram;
  GLuint drawNormalsProgram;
  GLuint bpDrawProgram;

  std::shared_ptr<ge::gl::Shader> csShader;
  std::shared_ptr<ge::gl::Shader> gsShader;
  std::shared_ptr<ge::gl::Shader> gsNormalShader;
  std::shared_ptr<ge::gl::Shader> gsCubeShader;
  std::shared_ptr<ge::gl::Shader> vsShaderLight;
  std::shared_ptr<ge::gl::Shader> vsShader;
  std::shared_ptr<ge::gl::Shader> vsNormalShader;
  std::shared_ptr<ge::gl::Shader> fsShaderLight;
  std::shared_ptr<ge::gl::Shader> fsShader;
  std::shared_ptr<ge::gl::Shader> vsBlinPhongShader;
  std::shared_ptr<ge::gl::Shader> fsBlinPhongShader;

  std::shared_ptr<ge::gl::Buffer> polyCountLUTBuffer;
  std::shared_ptr<ge::gl::Buffer> edgeLUTBuffer;
  std::shared_ptr<ge::gl::Buffer> edgeToVertexLUTBuffer;

  CameraController cameraController;
};
*/
#endif // TERRAINGENERATION_DENSITYGENERATORS_H
