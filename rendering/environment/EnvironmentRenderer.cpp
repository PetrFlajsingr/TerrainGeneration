//
// Created by petr on 12/27/19.
//

#include "EnvironmentRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace ShaderLiterals;
EnvironmentRenderer::EnvironmentRenderer(ObjModelLoader &loader) {
  cloudModel = loader.loadModel("floor", "floor");
  waterModel = loader.loadModel("floor", "floor");

  cloudModel->setScale(500);
  cloudModel->setPosition(0, 10000, 0);

  waterModel->setScale(500);
  waterModel->setPosition(0, -1000, 0);
  cloudProgram = std::make_shared<ge::gl::Program>("environment/clouds"_vert, "environment/clouds"_frag);
  waterProgram = std::make_shared<ge::gl::Program>("environment/water"_vert, "environment/water"_frag);
}

void EnvironmentRenderer::render(sdl2cpp::ui::CameraController &cameraController, float time) {
  ge::gl::glEnable(GL_BLEND);
  ge::gl::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glm::mat4 projection = cameraController.camera.projection.matrix;
  cloudModel->setPosition(cameraController.getPosition() + glm::vec3{0, cloudRelativePosition, 0});
  const glm::vec3 waterPos{waterModel->getPosition().x, waterLevel, waterModel->getPosition().z};
  waterModel->setPosition(waterPos);
  cloudProgram->use();

  cloudProgram->setMatrix4fv("model", glm::value_ptr(cloudModel->modelMatrix.getRef()));
  cloudProgram->setMatrix4fv("view", glm::value_ptr(cameraController.getViewMatrix()));
  cloudProgram->setMatrix4fv("projection", glm::value_ptr(projection));
  cloudProgram->set("time", time);

  cloudModel->getVertexArray()->bind();
  ge::gl::glDrawElements(GL_TRIANGLES, cloudModel->getElementBuffer()->getSize(), GL_UNSIGNED_INT, nullptr);

  return;
  waterProgram->use();

  waterProgram->setMatrix4fv("model", glm::value_ptr(waterModel->modelMatrix.getRef()));
  waterProgram->setMatrix4fv("view", glm::value_ptr(cameraController.getViewMatrix()));
  waterProgram->setMatrix4fv("projection", glm::value_ptr(projection));
  waterProgram->set("time", time);

  waterModel->getVertexArray()->bind();
  ge::gl::glDrawElements(GL_TRIANGLES, waterModel->getElementBuffer()->getSize(), GL_UNSIGNED_INT, nullptr);
}
void EnvironmentRenderer::setCloudRelativePosition(float cloudRelativePosition) {
  EnvironmentRenderer::cloudRelativePosition = cloudRelativePosition;
}
void EnvironmentRenderer::setWaterLevel(float waterLevel) { EnvironmentRenderer::waterLevel = waterLevel; }
