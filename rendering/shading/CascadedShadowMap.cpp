//
// Created by petr on 12/8/19.
//

#include "CascadedShadowMap.h"
#include "utils/types/Range.h"
#include <geGL/StaticCalls.h>
#include <glm/gtc/matrix_transform.hpp>
#include <io/print.h>

using namespace MakeRange;

CascadedShadowMap::CascadedShadowMap(unsigned int cascadeCount,
                                     unsigned int textureWidth,
                                     unsigned int textureHeight)
    : cascadeCount(cascadeCount), width(textureWidth), height(textureHeight) {
  lightProjections.resize(cascadeCount);
  perspective.resize(cascadeCount);
  for ([[maybe_unused]] auto _ : range(cascadeCount)) {
    printErr("**** Please ignore the following errors ****");
    depthMaps.emplace_back(std::make_unique<ge::gl::Texture>(GL_TEXTURE_2D, GL_DEPTH_COMPONENT32, 0, static_cast<int>(width), static_cast<int>(height)));
    setupTexture(*depthMaps.back());
    printErr("****************************************");
  }

  ge::gl::glFramebufferDrawBufferEXT(depthMapFBO.getId(), GL_NONE);
  ge::gl::glFramebufferReadBufferEXT(depthMapFBO.getId(), GL_NONE);
}

void CascadedShadowMap::bindCascade(unsigned int index) {
  depthMapFBO.attachTexture(GL_DEPTH_ATTACHMENT, depthMaps[index].get());
}

void CascadedShadowMap::bindRender(const std::shared_ptr<ge::gl::Program> &program) {
  for (auto i : range(depthMaps.size())) {
    ge::gl::glUniform1i(
        ge::gl::glGetUniformLocation(program->getId(), ("shadowMap[" + std::to_string(i) + "]").c_str()), depthMaps[i]->getId());
    ge::gl::glUniform1f(ge::gl::glGetUniformLocation(program->getId(), ("cascadeEnd[" + std::to_string(i) + "]").c_str()), cascadeBoundaries[i + 1]);
    ge::gl::glUniformMatrix4fv(ge::gl::glGetUniformLocation(program->getId(), ("lightSpaceMatrix[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &lightSpaceMatrix(i)[0][0]);
    //depthMaps[i]->bind(shadowTextureUnitOffset + i);
  }
}

glm::mat4 CascadedShadowMap::calculateOrthoMatrices(const glm::mat4 &cameraView,
                                                    float cameraNear,
                                                    float cameraFar,
                                                    float aspectRatio,
                                                    float fieldOfView) {
  cascadeBoundaries.resize(cascadeCount + 1);
  const auto max = static_cast<float>(cascadeCount + 1);
  for (auto i : range(1, cascadeCount + 1, 1)) {
    cascadeBoundaries[i-1] = cameraNear + i / max * (cameraFar - cameraNear);
  }

  const auto invertedCameraView = glm::inverse(cameraView);
  const auto lightView = calcLightView();
  const auto tanHalfHorizontalFOV = glm::tan(glm::radians(fieldOfView / 2.0f));
  const auto tanHalfVerticalFOV =
      glm::tan(glm::radians((fieldOfView * aspectRatio / 2.0f)));

  for (auto i : range(cascadeCount)) {
    const auto xn = cascadeBoundaries[i] * tanHalfHorizontalFOV;
    const auto xf = cascadeBoundaries[i + 1] * tanHalfHorizontalFOV;
    const auto yn = cascadeBoundaries[i] * tanHalfVerticalFOV;
    const auto yf = cascadeBoundaries[i + 1] * tanHalfVerticalFOV;

    auto frustumCorners =
        std::vector{glm::vec4{xn, yn, cascadeBoundaries[i], 1.0},
                    {-xn, yn, cascadeBoundaries[i], 1.0},
                    {xn, -yn, cascadeBoundaries[i], 1.0},
                    {-xn, -yn, cascadeBoundaries[i], 1.0},

                    {xf, yf, cascadeBoundaries[i + 1], 1.0},
                    {-xf, yf, cascadeBoundaries[i + 1], 1.0},
                    {xf, -yf, cascadeBoundaries[i + 1], 1.0},
                    {-xf, -yf, cascadeBoundaries[i + 1], 1.0}};

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();

    for (auto &corner : frustumCorners) {
      const glm::vec4 vW = invertedCameraView * corner;
      corner = lightView * vW;

      minX = glm::min(minX, corner.x);
      maxX = glm::max(maxX, corner.x);
      minY = glm::min(minY, corner.y);
      maxY = glm::max(maxY, corner.y);
      minZ = glm::min(minZ, corner.z);
      maxZ = glm::max(maxZ, corner.z);
    }
    perspective[i] = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
  }
}
glm::mat4 CascadedShadowMap::lightSpaceMatrix(unsigned int cascade) const {
  return perspective[cascade] * calcLightView();
}
const std::vector<std::unique_ptr<ge::gl::Texture>> &
CascadedShadowMap::getDepthMaps() const {
  return depthMaps;
}
const glm::vec3 &CascadedShadowMap::getLightDir() const { return lightDir; }
void CascadedShadowMap::setLightDir(const glm::vec3 &lightDir) {
  CascadedShadowMap::lightDir = lightDir;
}
const glm::vec3 &CascadedShadowMap::getLightPos() const { return lightPos; }
void CascadedShadowMap::setLightPos(const glm::vec3 &lightPos) {
  CascadedShadowMap::lightPos = lightPos;
}
glm::mat4 CascadedShadowMap::calcLightView() const {
  return glm::lookAt({0, 0, 0}, lightDir, {0, 1, 0});
}

void CascadedShadowMap::setupTexture(ge::gl::Texture &texture) {
  glm::vec4 borderColor{1.0, 1.0, 1.0, 1.0};
  ge::gl::glTextureImage2DEXT(texture.getId(), GL_TEXTURE_2D, 0,
                              GL_DEPTH_COMPONENT32, width, height, 0,
                              GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  texture.texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture.texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  texture.texParameteri(GL_TEXTURE_COMPARE_MODE, GL_NONE);
  texture.texParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  texture.texParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  depthMaps.back()->texParameterfv(GL_TEXTURE_BORDER_COLOR, &borderColor[0]);
}
