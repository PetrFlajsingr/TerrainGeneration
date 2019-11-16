//
// Created by petr on 10/20/19.
//

#include "CameraController.h"

CameraController::EventCallback CameraController::getKeyboardCallback() {
  return [this](const SDL_Event &event) {
    if (lockedToCamera) {
      auto key = event.key.keysym.sym;
      switch (key) {
      case SDLK_w:
        camera.ProcessKeyboard(FORWARD, 1);
        break;
      case SDLK_s:
        camera.ProcessKeyboard(BACKWARD, 1);
        break;
      case SDLK_a:
        camera.ProcessKeyboard(LEFT, 1);
        break;
      case SDLK_d:
        camera.ProcessKeyboard(RIGHT, 1);
        break;
      default:
        break;
      }
      return true;
    }
    return false;
  };
}
CameraController::EventCallback CameraController::getMouseDownCallback() {
  return [this](const SDL_Event &event) {
    if (event.button.button == SDL_BUTTON_RIGHT) {
      lockedToCamera = true;
      SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    return true;
  };
}
CameraController::EventCallback CameraController::getMouseMoveCallback() {
  return [this](const SDL_Event &event) {
    if (lockedToCamera) {
      camera.ProcessMouseMovement(event.motion.xrel, -event.motion.yrel, true);
      return true;
    }
    return false;
  };
}
CameraController::EventCallback CameraController::getMouseUpCallback() {
  return [this](const SDL_Event &event) {
    if (event.button.button == SDL_BUTTON_RIGHT) {
      lockedToCamera = false;
      SDL_SetRelativeMouseMode(SDL_FALSE);
    }
    return true;
  };
}
