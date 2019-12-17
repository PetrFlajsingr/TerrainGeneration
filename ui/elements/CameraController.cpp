//
// Created by petr on 10/20/19.
//

#include "CameraController.h"

#include <utility>

sdl2cpp::ui::CameraController::CameraController(UIManager &guiManager,
                                                PerspectiveProjection projection,
                                                glm::vec3 position,
                                                glm::vec3 dimensions,
                                                glm::vec3 startingPosition,
                                                glm::vec3 direction)
    : UIObject(guiManager), UIVisible(position, dimensions),
      camera(std::move(projection), startingPosition) {
  camera.MovementSpeed = 5.f;
}

void sdl2cpp::ui::CameraController::onMouseDown(const SDL_Event &event) {
  if (event.button.button == SDL_BUTTON_RIGHT) {
    lockedToCamera = true;
    setFocus(sdl2cpp::ui::Focus::Focused);
    SDL_SetRelativeMouseMode(SDL_TRUE);
  }
}

void sdl2cpp::ui::CameraController::onMouseUp(const SDL_Event &event) {
  if (event.button.button == SDL_BUTTON_RIGHT) {
    lockedToCamera = false;
    setFocus(sdl2cpp::ui::Focus::NotFocused);
    SDL_SetRelativeMouseMode(SDL_FALSE);
  }
}

void sdl2cpp::ui::CameraController::onMouseMove(const SDL_Event &event) {
  if (lockedToCamera) {
    camera.ProcessMouseMovement(event.motion.xrel, -event.motion.yrel, true);
  }
}

void sdl2cpp::ui::CameraController::onKeyDown(const SDL_Event &event) {
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

glm::vec3 sdl2cpp::ui::CameraController::getPosition() const {
  return camera.Position;
}

glm::mat4 sdl2cpp::ui::CameraController::getViewMatrix() {
  return camera.GetViewMatrix();
}

void sdl2cpp::ui::CameraController::draw(sdl2cpp::ui::GUIRenderer &renderer) {}

void sdl2cpp::ui::CameraController::onVisibilityChanged(
    sdl2cpp::ui::Visibility visibility) {}

void sdl2cpp::ui::CameraController::onFocusChanged(sdl2cpp::ui::Focus focus) {}

void sdl2cpp::ui::CameraController::onEnabledChanged(bool enabled) {}

void sdl2cpp::ui::CameraController::onMouseClicked(const SDL_Event &event) {}

void sdl2cpp::ui::CameraController::onMouseDblClicked(const SDL_Event &event) {}

void sdl2cpp::ui::CameraController::onMouseOver(const SDL_Event &event) {}

void sdl2cpp::ui::CameraController::onMouseOut(const SDL_Event &event) {}

void sdl2cpp::ui::CameraController::onMouseWheel(const SDL_Event &event) {
    camera.ProcessMouseScroll(event.wheel.y);
}
