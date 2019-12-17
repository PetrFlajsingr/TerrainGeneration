//
// Created by petr on 12/8/19.
//

#include "CascadedShadowMap.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace MakeRange;
using ShaderLiterals::operator""_vert;
using ShaderLiterals::operator""_frag;

CascadedShadowMap::CascadedShadowMap(unsigned int cascadeCount,
                                     unsigned int size)
    : cascadeCount(cascadeCount), size(size), program(std::make_shared<ge::gl::Program>(
    "shadow_map/sm"_vert, "shadow_map/sm"_frag)) {
  lightViewMatrix.resize(cascadeCount);
  lightOrthoMatrix.resize(cascadeCount);
  cascadedMatrices.resize(cascadeCount);
  cascadeSplitArray.resize(cascadeCount);
  bbs.resize(cascadeCount);

  ge::gl::glGenFramebuffers(1, &depthMapFBO);
  ge::gl::glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

  ge::gl::glGenTextures(1, &depthMap);
  ge::gl::glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap);

  ge::gl::glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap);
  ge::gl::glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, size,
                       size, cascadeCount, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                       nullptr);
  ge::gl::glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
                          GL_LINEAR);
  ge::gl::glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER,
                          GL_LINEAR);
  ge::gl::glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S,
                          GL_CLAMP_TO_EDGE);
  ge::gl::glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T,
                          GL_CLAMP_TO_EDGE);
  ge::gl::glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE,
                          GL_COMPARE_R_TO_TEXTURE);
  ge::gl::glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC,
                          GL_LEQUAL);

  ge::gl::glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap,
                               0);
  ge::gl::glDrawBuffer(GL_NONE);
  ge::gl::glReadBuffer(GL_NONE);
  ge::gl::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void CascadedShadowMap::bindCascade(unsigned int index) {
  ge::gl::glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                    depthMap, 0, index);
}

void CascadedShadowMap::bindRender(
    const std::shared_ptr<ge::gl::Program> &program) {
  ge::gl::glActiveTexture(GL_TEXTURE0);
  ge::gl::glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap);
  ge::gl::glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE,
                          GL_COMPARE_R_TO_TEXTURE);
  for (auto i : range(cascadeCount)) {
    ge::gl::glUniformMatrix4fv(
        ge::gl::glGetUniformLocation(
            program->getId(),
            ("lightViewProjectionMatrices[" + std::to_string(i) + "]").c_str()),
        1, GL_FALSE, &cascadedMatrices[i][0][0]);
  }
  program->set4fv("cascadedSplits", getCascadeSplits().data());
  program->set1i("numOfCascades", getCascadeCount());
}

void CascadedShadowMap::calculateOrthoMatrices(
    const glm::mat4 &cameraProjection, const glm::mat4 &cameraView,
    float cameraNear, float cameraFar, float aspectRatio, float fieldOfView) {
  std::vector<float> cascadeSplits;
  cascadeSplits.resize(cascadeCount);

  const float nearClip = cameraNear;
  const float farClip = cameraFar;
  const float clipRange = farClip - nearClip;

  const float minZ = nearClip + minDistance * clipRange;
  const float maxZ = nearClip + maxDistance * clipRange;

  const float zRange = maxZ - minZ;
  const float ratio = maxZ / minZ;

  for (auto i : range(cascadeCount)) {
    float p = (i + 1.f) / static_cast<float>(cascadeCount);
    float log = minZ * std::pow(ratio, p);
    float uniform = minZ + zRange * p;
    float d = lambda * (log - uniform) + uniform;
    cascadeSplits[i] = (d - nearClip) / clipRange;
  }

  for (unsigned int cascadeIterator = 0; cascadeIterator < cascadeCount;
       ++cascadeIterator) {
    const float prevSplitDistance =
        cascadeIterator == 0 ? minDistance : cascadeSplits[cascadeIterator - 1];
    const float splitDistance = cascadeSplits[cascadeIterator];

    glm::vec3 frustumCornersWS[8] = {
        glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),  glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),  glm::vec3(-1.0f, -1.0f, 1.0f),
    };

    const glm::mat4 invViewProj = glm::inverse(cameraProjection * cameraView);
    for (auto &corner : frustumCornersWS) {
      glm::vec4 inversePoint = invViewProj * glm::vec4(corner, 1.0f);
      corner = glm::vec3(inversePoint / inversePoint.w);
    }

    for (auto i : range(4)) {
      const glm::vec3 cornerRay = frustumCornersWS[i + 4] - frustumCornersWS[i];
      const glm::vec3 nearCornerRay = cornerRay * prevSplitDistance;
      const glm::vec3 farCornerRay = cornerRay * splitDistance;
      frustumCornersWS[i + 4] = frustumCornersWS[i] + farCornerRay;
      frustumCornersWS[i] = frustumCornersWS[i] + nearCornerRay;
    }

    auto frustumCenter = glm::vec3(0.0f);
    for (auto corner : frustumCornersWS) {
      frustumCenter += corner;
    }
    frustumCenter /= 8.0f;

    float radius = 0.0f;
    for (auto corner : frustumCornersWS) {
      float distance = glm::length(corner - frustumCenter);
      radius = glm::max(radius, distance);
    }
    radius = std::ceil(radius * 16.0f) / 16.0f;

    const glm::vec3 maxExtents = glm::vec3(radius, radius, radius);
    const glm::vec3 minExtents = -maxExtents;

    const glm::vec3 lightDirection =
        frustumCenter - glm::normalize(lightDir) * -minExtents.z;
    lightViewMatrix[cascadeIterator] = glm::mat4(1.0f);
    lightViewMatrix[cascadeIterator] =
        glm::lookAt(lightDirection, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));

    const glm::vec3 cascadeExtents = maxExtents - minExtents;

    lightOrthoMatrix[cascadeIterator] =
        glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f,
                   cascadeExtents.z);

    bbs[cascadeIterator] = geo::BoundingBox{minExtents, maxExtents};

    const auto shadowMatrix =
        lightOrthoMatrix[cascadeIterator] * lightViewMatrix[cascadeIterator];
    auto shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    shadowOrigin = shadowMatrix * shadowOrigin;
    shadowOrigin = shadowOrigin * static_cast<float>(size) / 2.0f;

    const glm::vec4 roundedOrigin = glm::round(shadowOrigin);
    glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
    roundOffset = roundOffset * 2.0f / static_cast<float>(size);
    roundOffset.z = 0.0f;
    roundOffset.w = 0.0f;

    glm::mat4 shadowProj = lightOrthoMatrix[cascadeIterator];
    shadowProj[3] += roundOffset;
    lightOrthoMatrix[cascadeIterator] = shadowProj;

    const float clipDist = cameraFar - cameraNear;
    cascadeSplitArray[cascadeIterator] =
        (cameraNear + splitDistance * clipDist) * -1.0f;

    cascadedMatrices[cascadeIterator] =
        lightOrthoMatrix[cascadeIterator] * lightViewMatrix[cascadeIterator];
  }
}

const glm::vec3 &CascadedShadowMap::getLightDir() const { return lightDir; }
void CascadedShadowMap::setLightDir(const glm::vec3 &lightDir) {
  CascadedShadowMap::lightDir = lightDir;
}

const std::vector<float> &CascadedShadowMap::getCascadeSplits() const {
  return cascadeSplitArray;
}
unsigned int CascadedShadowMap::getCascadeCount() const { return cascadeCount; }

GLuint CascadedShadowMap::getDepthMap() const { return depthMap; }

float CascadedShadowMap::getLambda() const { return lambda; }
void CascadedShadowMap::setLambda(float lambda) {
  CascadedShadowMap::lambda = lambda;
}

float CascadedShadowMap::getMinDistance() const { return minDistance; }
void CascadedShadowMap::setMinDistance(float minDistance) {
  CascadedShadowMap::minDistance = minDistance;
}

float CascadedShadowMap::getMaxDistance() const { return maxDistance; }
void CascadedShadowMap::setMaxDistance(float maxDistance) {
  CascadedShadowMap::maxDistance = maxDistance;
}
