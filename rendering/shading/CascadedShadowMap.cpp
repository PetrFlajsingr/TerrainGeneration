//
// Created by petr on 12/8/19.
//

#include "CascadedShadowMap.h"
#include "utils/types/Range.h"
#include <geGL/StaticCalls.h>

using namespace MakeRange;

CascadedShadowMap::CascadedShadowMap(unsigned int cascadeCount,
                                     unsigned int textureWidth,
                                     unsigned int textureHeight)
    : cascadeCount(cascadeCount), width(textureWidth), height(textureHeight) {
  lightProjections.resize(cascadeCount);
  glm::vec4 borderColor{1.0, 1.0, 1.0, 1.0};
  for ([[maybe_unused]] auto _ : range(cascadeCount)) {
    depthMaps.emplace_back(GL_TEXTURE_2D, GL_DEPTH_COMPONENT, 0, width, height);
    ge::gl::glTextureImage2DEXT(depthMaps.back().getId(), GL_TEXTURE_2D, 0,
                                GL_DEPTH_COMPONENT, width, height, 0,
                                GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    depthMaps.back().texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    depthMaps.back().texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    depthMaps.back().texParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    depthMaps.back().texParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    depthMaps.back().texParameterfv(GL_TEXTURE_BORDER_COLOR, &borderColor[0]);
  }

  ge::gl::glFramebufferDrawBufferEXT(depthMapFBO.getId(), GL_NONE);
  ge::gl::glFramebufferReadBufferEXT(depthMapFBO.getId(), GL_NONE);
}

void CascadedShadowMap::bindCascade(unsigned int index) {
  depthMapFBO.attachTexture(GL_DEPTH_ATTACHMENT, &depthMaps[index]);
}

void CascadedShadowMap::bindRender() {
  for (auto i : range(depthMaps.size())) {
    depthMaps[i].bind(shadowTextureUnitOffset + i);
  }
}
