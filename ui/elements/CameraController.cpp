//
// Created by petr on 10/20/19.
//

#include "CameraController.h"
#include "io/print.h"

CameraController::CameraController(SDL_Rect area, glm::vec3 startingPosition,
                             glm::vec3 direction)
    : UIVisible(area), camera(startingPosition) {
  camera.MovementSpeed = 0.7f;
}

void CameraController::onMouseDown(const SDL_Event &event) {
  if (event.button.button == SDL_BUTTON_RIGHT) {
    lockedToCamera = true;
    enableKeyboardInput();
    SDL_SetRelativeMouseMode(SDL_TRUE);
  }
}
void CameraController::onMouseUp(const SDL_Event &event) {
  if (event.button.button == SDL_BUTTON_RIGHT) {
    lockedToCamera = false;
    disableKeyboardInput();
    SDL_SetRelativeMouseMode(SDL_FALSE);
  }
}
void CameraController::onMouseMove(const SDL_Event &event) {
  if (lockedToCamera) {
    camera.ProcessMouseMovement(event.motion.xrel, -event.motion.yrel, true);
  }
}
void CameraController::onKeyPressed(const SDL_Event &event) {}
void CameraController::onKeyDown(const SDL_Event &event) {
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
}
glm::vec3 CameraController::getPosition() const { return camera.Position; }

glm::mat4 CameraController::getViewMatrix() { return camera.GetViewMatrix(); }
void CameraController::draw() {}
void CameraController::onVisibilityChanged(
    sdl2cpp::ui::Visibility visibility) {}
void CameraController::onFocusChanged(sdl2cpp::ui::Focus focus) {}
void CameraController::onEnabledChanged(bool enabled) {}
void CameraController::onMouseClicked(const SDL_Event &event) {}
void CameraController::onMouseDblClicked(const SDL_Event &event) {}
void CameraController::onMouseOver(const SDL_Event &event) {}
void CameraController::onMouseOut(const SDL_Event &event) {}
void CameraController::onKeyUp(const SDL_Event &event) {}
