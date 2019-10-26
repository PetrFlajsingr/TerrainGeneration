//
// Created by petr on 10/20/19.
//

#ifndef TERRAINGENERATION_DENSITYGENERATORS_H
#define TERRAINGENERATION_DENSITYGENERATORS_H

#include "../gui/CameraController.h"
#include "Chunk.h"
#include "lookuptables.h"
#include "shaders/GlslShaderLoader.h"
#include <functional>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <optional>
#include <string>

#define CHUNK_LEN 32
#define CHUNK_SIZE (CHUNK_LEN * CHUNK_LEN * CHUNK_LEN)

#define DRAW_MESH0
#define DRAW_NORMALS0
#define DRAW_SKELETON0

namespace {
struct AxisInterval {
  float lowerBound;
  float higherBound;
  uint parts;

  uint getCellCount() { return parts; }
};

struct AreaInterval {
  AxisInterval xInterval;
  AxisInterval yInterval;
  AxisInterval zInterval;

  uint getCellCount() {
    return xInterval.getCellCount() * yInterval.getCellCount() *
           zInterval.getCellCount();
  }
};

} // namespace

struct Compute {
  const float d = 1/1.f;
  const float w = 1 / (d * 32.0);
  std::vector<mc::Chunk> chunks; /*{{32, {0/d-0*w, 0/d-0*w, 0/d-0*w}},
                                 {32, {0/d-0*w, 0/d-0*w, 1/d-1*w}},
                                 {32, {0/d-0*w, 0/d-0*w, 2/d-2*w}},
                                 {32, {0/d-0*w, 1/d-1*w, 0/d-0*w}},
                                 {32, {0/d-0*w, 1/d-1*w, 1/d-1*w}},
                                 {32, {0/d-0*w, 1/d-1*w, 2/d-2*w}},
                                 {32, {0/d-0*w, 2/d-2*w, 0/d-0*w}},
                                 {32, {0/d-0*w, 2/d-2*w, 1/d-1*w}},
                                 {32, {0/d-0*w, 2/d-2*w, 2/d-2*w}},
                                 {32, {1/d-1*w, 0/d-0*w, 0/d-0*w}},
                                 {32, {1/d-1*w, 0/d-0*w, 1/d-1*w}},
                                 {32, {1/d-1*w, 0/d-0*w, 2/d-2*w}},
                                 {32, {1/d-1*w, 1/d-1*w, 0/d-0*w}},
                                 {32, {1/d-1*w, 1/d-1*w, 1/d-1*w}},
                                 {32, {1/d-1*w, 1/d-1*w, 2/d-2*w}},
                                 {32, {1/d-1*w, 2/d-2*w, 0/d-0*w}},
                                 {32, {1/d-1*w, 2/d-2*w, 1/d-1*w}},
                                 {32, {1/d-1*w, 2/d-2*w, 2/d-2*w}},
                                 {32, {2/d-2*w, 0/d-0*w, 0/d-0*w}},
                                 {32, {2/d-2*w, 0/d-0*w, 1/d-1*w}},
                                 {32, {2/d-2*w, 0/d-0*w, 2/d-2*w}},
                                 {32, {2/d-2*w, 1/d-1*w, 0/d-0*w}},
                                 {32, {2/d-2*w, 1/d-1*w, 1/d-1*w}},
                                 {32, {2/d-2*w, 1/d-1*w, 2/d-2*w}},
                                 {32, {2/d-2*w, 2/d-2*w, 1/d-1*w}},
                                 {32, {2/d-2*w, 2/d-2*w, 2/d-2*w}},
                                 };*/

  void generateChunks() {
    for (int i = 0; i < 14; i+=2) {
      for (int j = 0; j < 14; j+=2) {
        for (int k = 0; k < 14; k+=2) {
          chunks.emplace_back(
              32, 2.0f, glm::vec3{i / d - i * w, j / d - j * w, k / d - k * w}, glm::vec4(i/7.f,j/7.f,k/7.f, 1));
        }
      }
    }
  }

  Compute(AreaInterval areaInterval) : areaInterval(areaInterval) {
    generateChunks();
    GLint isLinked;
    csShader = std::make_shared<ge::gl::Shader>(GL_COMPUTE_SHADER,
                                                "mc_chunk.comp"_shader_file);
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

    drawNormalsProgram = ge::gl::glCreateProgram();

    gsNormalShader = std::make_shared<ge::gl::Shader>(
        GL_GEOMETRY_SHADER, "normal_to_line.geom"_shader_file);
    vsNormalShader = std::make_shared<ge::gl::Shader>(
        GL_VERTEX_SHADER, "normal_to_line.vert"_shader_file);
    ge::gl::glAttachShader(drawNormalsProgram, gsNormalShader->getId());
    ge::gl::glAttachShader(drawNormalsProgram, vsNormalShader->getId());
    ge::gl::glAttachShader(drawNormalsProgram, fsShader->getId());
    ge::gl::glLinkProgram(drawNormalsProgram);

    ge::gl::glGetProgramiv(drawNormalsProgram, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
      int length;
      std::string log;
      ge::gl::glGetProgramiv(drawNormalsProgram, GL_INFO_LOG_LENGTH, &length);
      log.resize(static_cast<unsigned long>(length));
      ge::gl::glGetProgramInfoLog(drawNormalsProgram, length, &length, &log[0]);

      // The program is useless now. So delete it.
      ge::gl::glDeleteProgram(drawNormalsProgram);
      std::cerr << "cannot Link: " << log << std::endl;
      throw std::runtime_error("cannot link opengl program");
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
    ge::gl::glGetProgramiv(chunkSkeletonDrawProgram, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
      int length;
      std::string log;
      ge::gl::glGetProgramiv(chunkSkeletonDrawProgram, GL_INFO_LOG_LENGTH,
                             &length);
      log.resize(static_cast<unsigned long>(length));
      ge::gl::glGetProgramInfoLog(chunkSkeletonDrawProgram, length, &length,
                                  &log[0]);

      // The program is useless now. So delete it.
      ge::gl::glDeleteProgram(chunkSkeletonDrawProgram);
      std::cerr << "cannot Link: " << log << std::endl;
      throw std::runtime_error("cannot link opengl program");
    }

    // const GLchar *varyings[] = {"gl_Position", "normal"};
    const GLchar *varyings[] = {"Wat.pos", "Wat.normal"};
    ge::gl::glTransformFeedbackVaryings(gsProgram, 2, varyings,
                                        GL_SEPARATE_ATTRIBS);
    ge::gl::glLinkProgram(gsProgram);

    ge::gl::glGetProgramiv(gsProgram, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
      int length;
      std::string log;
      ge::gl::glGetProgramiv(gsProgram, GL_INFO_LOG_LENGTH, &length);
      log.resize(static_cast<unsigned long>(length));
      ge::gl::glGetProgramInfoLog(gsProgram, length, &length, &log[0]);

      // The program is useless now. So delete it.
      ge::gl::glDeleteProgram(gsProgram);
      std::cerr << "cannot Link: " << log << std::endl;
      throw std::runtime_error("cannot link opengl program");
    }

    // vb = std::make_shared<ge::gl::Buffer>(CHUNK_SIZE * sizeof(float) * 4,
    //                                      /*vertices.data()*/nullptr,
    //                                      GL_DYNAMIC_DRAW);
    // densityBuffer = std::make_shared<ge::gl::Buffer>(
    //    CHUNK_SIZE * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    // cubeIndexBuffer = std::make_shared<ge::gl::Buffer>(
    //    CHUNK_SIZE * sizeof(uint), nullptr, GL_DYNAMIC_DRAW);
    // geometryQuery = std::make_shared<ge::gl::AsynchronousQuery>(
    //    GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, GL_QUERY_RESULT,
    //    ge::gl::AsynchronousQuery::UINT32);

    polyCountLUTBuffer = std::make_shared<ge::gl::Buffer>(
        mc::LUT::polygonCount.size() * sizeof(uint32_t),
        mc::LUT::polygonCount.data(), GL_STATIC_COPY);
    edgeLUTBuffer = std::make_shared<ge::gl::Buffer>(
        mc::LUT::edges.size() * sizeof(uint32_t) * 3 * 5, mc::LUT::edges.data(),
        GL_STATIC_COPY);
    edgeToVertexLUTBuffer = std::make_shared<ge::gl::Buffer>(
        mc::LUT::edgeToVertexIds.size() * sizeof(uint32_t) * 2,
        mc::LUT::edgeToVertexIds.data(), GL_STATIC_COPY);

    // vertexFeedbackBuffer = std::make_shared<ge::gl::Buffer>(
    //    CHUNK_SIZE * 5 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    //
    // normalFeedbackBuffer = std::make_shared<ge::gl::Buffer>(CHUNK_SIZE * 5 *
    // 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    //
    // vao = std::make_shared<ge::gl::VertexArray>();
    // vao->addAttrib(vb, 0, 4, GL_FLOAT, static_cast<GLsizei>(sizeof(float) *
    // 4),
    //               0);
    //
    // vao2 = std::make_shared<ge::gl::VertexArray>();
    // vao2->addAttrib(vertexFeedbackBuffer, 0, 4, GL_FLOAT,
    // static_cast<GLsizei>(sizeof(float) * 4),
    //                0);
    // vao2->addAttrib(normalFeedbackBuffer, 1, 3, GL_FLOAT,
    // static_cast<GLsizei>(sizeof(float) * 3),
    //                0);

    // ge::gl::glGenTransformFeedbacks(1, &feedbackName);
    // ge::gl::glGenTransformFeedbacks(1, &normalFeedbackName);

    // ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackName);
    // ge::gl::glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0,
    //                         vertexFeedbackBuffer->getId());
    ////ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    //
    ////ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackName);
    // ge::gl::glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1,
    //                         normalFeedbackBuffer->getId());
    // ge::gl::glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

    cameraController = CameraController(glm::vec3{.5f, .5f, 2.f});
  }

  void operator()() {
    const uint NAH = 2413564;
    static float offset = 0;
    offset += 0.002;
    if (std::any_of(chunks.begin(), chunks.end(),
                    [](const auto &a) { return !a.isComputed(); })) {

      std::cout << "Running compute on chunk" << std::endl;
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
          if (chunk.getId() == NAH)
            continue;
          chunk.dispatchDensityComputation(csProgram);
        }
      }

      ge::gl::glUniform1ui(uni, 1);

      for (auto &chunk : chunks) {
        if (!chunk.isComputed()) {
          if (chunk.getId() == NAH)
            continue;
          chunk.dispatchCubeIndicesComputation(csProgram);
        }
      }

      ge::gl::glUseProgram(gsProgram);
      ge::gl::glEnable(GL_RASTERIZER_DISCARD);

      polyCountLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
      edgeLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
      edgeToVertexLUTBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 3);

      for (auto &chunk : chunks) {
        if (!chunk.isComputed()) {
          if (chunk.getId() == NAH)
            continue;
          chunk.calculateVertices(gsProgram);
        }
      }
      ge::gl::glDisable(GL_RASTERIZER_DISCARD);
    }

    auto view = cameraController.getViewMatrix();
    auto model = glm::mat4();
    auto projection =
        glm::perspective(glm::radians(45.f), 1920.f / 1080, 0.1f, 100.0f);
    auto MVPmatrix = projection * view * model;

    glm::vec4 red{1, 0, 0, 1};

#ifdef DRAW_SKELETON
    ge::gl::glUseProgram(chunkSkeletonDrawProgram);

    auto colorUni =
        ge::gl::glGetUniformLocation(chunkSkeletonDrawProgram, "color");
    ge::gl::glUniformMatrix4fv(
        ge::gl::glGetUniformLocation(chunkSkeletonDrawProgram, "mvpUniform"), 1,
        GL_FALSE, &MVPmatrix[0][0]);

    ge::gl::glLineWidth(0.01f);
    ge::gl::glUniform4fv(colorUni, 1, &red[0]);

    for (auto &chunk : chunks) {
      if (chunk.shouldBeDrawn()) {
        if (chunk.getId() == NAH)
          continue;
        chunk.vertexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
        ge::gl::glDrawArrays(GL_POINTS, 0, CHUNK_SIZE);
      }
    }

#endif
    if (std::any_of(chunks.begin(), chunks.end(),
                    std::mem_fn(&mc::Chunk::shouldBeDrawn))) {
      ge::gl::glUseProgram(drawProgram);

      // ge::gl::glEnable(GL_CULL_FACE);
      ge::gl::glEnable(GL_DEPTH_TEST);

      auto lightPosUni = ge::gl::glGetUniformLocation(drawProgram, "lightPos");
      glm::vec3 lightPos = cameraController.camera.Position; //(1, 3, 1);
      ge::gl::glUniform3fv(lightPosUni, 1, &lightPos[0]);

      auto cameraPosUni =
          ge::gl::glGetUniformLocation(drawProgram, "cameraPos");
      ge::gl::glUniform3fv(cameraPosUni, 1,
                           &cameraController.camera.Position[0]);
      auto mvpMatrixUniformId =
          ge::gl::glGetUniformLocation(drawProgram, "mvpUniform");
      auto colorId = ge::gl::glGetUniformLocation(drawProgram, "color");
      ge::gl::glUniformMatrix4fv(mvpMatrixUniformId, 1, GL_FALSE,
                                 &MVPmatrix[0][0]);
#ifdef DRAW_MESH
      ge::gl::glLineWidth(5.f);
      glm::vec4 blue(0, 0, 1, 1);
      ge::gl::glUniform4fv(colorId, 1, &blue[0]);
      for (auto &chunk : chunks) {
        if (chunk.shouldBeDrawn()) {
          if (chunk.getId() == NAH)
            continue;
          ge::gl::glDrawTransformFeedback(GL_LINES, chunk.feedbackName);
        }
      }

      ge::gl::glLineWidth(1.f);
#endif

      for (auto &chunk : chunks) {
        if (chunk.shouldBeDrawn()) {
          if (chunk.getId() == NAH)
            continue;
          chunk.render(drawProgram);
        }
      }
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
      if (chunk.shouldBeDrawn()) {
        if (chunk.getId() == 2)
          continue;
        ge::gl::glDrawTransformFeedback(GL_POINTS, chunk.feedbackName);
      }
    }
#endif
    for (auto &chunk : chunks) {
      //chunk.invalidate();
    }
    ge::gl::glFlush();
  }
  // GLuint csProgram;
  GLuint csProgram;
  GLuint gsProgram;
  GLuint drawProgram;
  GLuint feedbackName;
  GLuint chunkSkeletonDrawProgram;
  GLuint drawNormalsProgram;

  std::shared_ptr<ge::gl::Shader> csShader;
  std::shared_ptr<ge::gl::Shader> gsShader;
  std::shared_ptr<ge::gl::Shader> gsNormalShader;
  std::shared_ptr<ge::gl::Shader> gsCubeShader;
  std::shared_ptr<ge::gl::Shader> vsShaderLight;
  std::shared_ptr<ge::gl::Shader> vsShader;
  std::shared_ptr<ge::gl::Shader> vsNormalShader;
  std::shared_ptr<ge::gl::Shader> fsShaderLight;
  std::shared_ptr<ge::gl::Shader> fsShader;

  // std::shared_ptr<ge::gl::Buffer> vb;
  // std::shared_ptr<ge::gl::VertexArray> vao;
  // std::shared_ptr<ge::gl::VertexArray> vao2;
  // std::shared_ptr<ge::gl::Buffer> densityBuffer;
  // std::shared_ptr<ge::gl::Buffer> cubeIndexBuffer;
  // std::shared_ptr<ge::gl::Buffer> vertexFeedbackBuffer;
  // std::shared_ptr<ge::gl::Buffer> normalFeedbackBuffer;
  // std::vector<glm::vec4> vertices;

  // std::shared_ptr<ge::gl::AsynchronousQuery> geometryQuery;

  std::shared_ptr<ge::gl::Buffer> polyCountLUTBuffer;
  std::shared_ptr<ge::gl::Buffer> edgeLUTBuffer;
  std::shared_ptr<ge::gl::Buffer> edgeToVertexLUTBuffer;

  AreaInterval areaInterval;

  std::array<GLuint, 1> subroutines{0};

  CameraController cameraController;
};

#endif // TERRAINGENERATION_DENSITYGENERATORS_H
