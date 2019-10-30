//
// Created by petr on 10/20/19.
//

#ifndef TERRAINGENERATION_CAMERACONTROLLER_H
#define TERRAINGENERATION_CAMERACONTROLLER_H

#include <SDL_events.h>
#include <functional>
#include <Camera.h>

class CameraController {
public:
  using EventCallback = std::function<bool(const SDL_Event &)>;

  explicit CameraController(glm::vec3 startingPosition = {0.f, 0.f, 0.f},
      glm::vec3 direction = {0.f, 0.f, -1.f}) : camera(startingPosition) {
    camera.MovementSpeed = 0.05f;
  }

  EventCallback getKeyboardCallback();
  EventCallback getMouseDownCallback();
  EventCallback getMouseUpCallback();
  EventCallback getMouseMoveCallback();

  glm::mat4 getViewMatrix() {
    return camera.GetViewMatrix();
  }

  float movementSpeed = 0.1f;
//private:
  bool lockedToCamera = false;
  Camera camera;
};

#endif // TERRAINGENERATION_CAMERACONTROLLER_H
