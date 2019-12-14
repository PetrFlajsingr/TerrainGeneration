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
                                     unsigned int size)
    : cascadeCount(cascadeCount), size(size) {
  lightViewMatrix.resize(cascadeCount);
  lightOrthoMatrix.resize(cascadeCount);
  cascadedMatrices.resize(cascadeCount);
  cascadeSplitArray.resize(cascadeCount);

  ge::gl::glGenFramebuffers(1, &depthMapFBO);
  ge::gl::glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

  ge::gl::glGenTextures(1, &depthMap);
  ge::gl::glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap);

  ge::gl::glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap);
  ge::gl::glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, size,
                       size, cascadeCount, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                       NULL);
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
  // restore default FBO
  ge::gl::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void CascadedShadowMap::bindCascade(unsigned int index) {
  // depthMapFBO.attachTexture(GL_DEPTH_ATTACHMENT, depthMaps[index].get());
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

  // Between 0 and 1, change in order to see the results
  GLfloat lambda = 1.0f;

  // Between 0 and 1, change these to check the results
  GLfloat minDistance = 0.0f;
  GLfloat maxDistance = 1.0f;

  GLfloat nearClip = cameraNear;
  GLfloat farClip = cameraFar;
  GLfloat clipRange = farClip - nearClip;

  GLfloat minZ = nearClip + minDistance * clipRange;
  GLfloat maxZ = nearClip + maxDistance * clipRange;

  GLfloat zRange = maxZ - minZ;
  GLfloat ratio = maxZ / minZ;

  for (auto i : range(cascadeCount)) {
    GLfloat p = (i + 1.f) / static_cast<GLfloat>(cascadeCount);
    GLfloat log = minZ * std::pow(ratio, p);
    GLfloat uniform = minZ + zRange * p;
    GLfloat d = lambda * (log - uniform) + uniform;
    cascadeSplits[i] = (d - nearClip) / clipRange;
  }

  for (unsigned int cascadeIterator = 0; cascadeIterator < cascadeCount;
       ++cascadeIterator) {

    GLfloat prevSplitDistance =
        cascadeIterator == 0 ? minDistance : cascadeSplits[cascadeIterator - 1];
    GLfloat splitDistance = cascadeSplits[cascadeIterator];

    glm::vec3 frustumCornersWS[8] = {
        glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),  glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),  glm::vec3(-1.0f, -1.0f, 1.0f),
    };

    glm::mat4 invViewProj = glm::inverse(cameraProjection * cameraView);
    for (auto &corner : frustumCornersWS) {
      glm::vec4 inversePoint = invViewProj * glm::vec4(corner, 1.0f);
      corner = glm::vec3(inversePoint / inversePoint.w);
    }

    for (unsigned int i = 0; i < 4; ++i) {
      glm::vec3 cornerRay = frustumCornersWS[i + 4] - frustumCornersWS[i];
      glm::vec3 nearCornerRay = cornerRay * prevSplitDistance;
      glm::vec3 farCornerRay = cornerRay * splitDistance;
      frustumCornersWS[i + 4] = frustumCornersWS[i] + farCornerRay;
      frustumCornersWS[i] = frustumCornersWS[i] + nearCornerRay;
    }

    auto frustumCenter = glm::vec3(0.0f);
    for (auto corner : frustumCornersWS) {
      frustumCenter += corner;
    }
    frustumCenter /= 8.0f;

    GLfloat radius = 0.0f;
    for (auto corner : frustumCornersWS) {
      GLfloat distance = glm::length(corner - frustumCenter);
      radius = glm::max(radius, distance);
    }
    radius = std::ceil(radius * 16.0f) / 16.0f;

    glm::vec3 maxExtents = glm::vec3(radius, radius, radius);
    glm::vec3 minExtents = -maxExtents;

    // Position the viewmatrix looking down the center of the frustum with an
    // arbitrary lighht direction
    glm::vec3 lightDirection =
        frustumCenter - glm::normalize(lightDir) * -minExtents.z;
    lightViewMatrix[cascadeIterator] = glm::mat4(1.0f);
    lightViewMatrix[cascadeIterator] =
        glm::lookAt(lightDirection, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 cascadeExtents = maxExtents - minExtents;

    lightOrthoMatrix[cascadeIterator] =
        glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f,
                   cascadeExtents.z);

    // The rounding matrix that ensures that shadow edges do not shimmer
    glm::mat4 shadowMatrix =
        lightOrthoMatrix[cascadeIterator] * lightViewMatrix[cascadeIterator];
    glm::vec4 shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    shadowOrigin = shadowMatrix * shadowOrigin;
    shadowOrigin = shadowOrigin * static_cast<float>(size) / 2.0f;

    glm::vec4 roundedOrigin = glm::round(shadowOrigin);
    glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
    roundOffset = roundOffset * 2.0f / static_cast<float>(size);
    roundOffset.z = 0.0f;
    roundOffset.w = 0.0f;

    glm::mat4 shadowProj = lightOrthoMatrix[cascadeIterator];
    shadowProj[3] += roundOffset;
    lightOrthoMatrix[cascadeIterator] = shadowProj;

    // Store the split distances and the relevant matrices
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
