//
// Created by petr on 10/20/19.
//

#include "CameraController.h"

#include <utility>

sdl2cpp::ui::CameraController::CameraController(UIManager &guiManager, PerspectiveProjection projection, glm::vec3 position,
                                                glm::vec3 dimensions, glm::vec3 startingPosition, glm::vec3 direction)
    : UIObject(guiManager), UIVisible(position, dimensions), camera(std::move(projection), startingPosition) {
  camera.MovementSpeed = 5.f;
  camera.Front = direction;
}

void sdl2cpp::ui::CameraController::onMouseDown(MouseButton button, [[maybe_unused]] glm::vec2 position) {
  switch (button) {
  case MouseButton::Left:
    break;
  case MouseButton::Right:
    lockedToCamera = true;
    setFocus(sdl2cpp::ui::Focus::Focused);
    enableFullControl();
    SDL_SetRelativeMouseMode(SDL_TRUE);
    break;
  case MouseButton::Middle:
    break;
  }
}

void sdl2cpp::ui::CameraController::onMouseUp(MouseButton button, [[maybe_unused]] glm::vec2 position) {
  if (button == MouseButton::Right) {
    lockedToCamera = false;
    setFocus(sdl2cpp::ui::Focus::NotFocused);
    disableFullControl();
    SDL_SetRelativeMouseMode(SDL_FALSE);
  }
}

void sdl2cpp::ui::CameraController::onMouseMove(glm::vec2 newPos, glm::vec2 oldPos) {
  if (lockedToCamera) {
    camera.ProcessMouseMovement(static_cast<float>(newPos.x - oldPos.x), static_cast<float>(-(newPos.y - oldPos.y)), true);
  }
}

void sdl2cpp::ui::CameraController::onKeyDown(SDL_Keycode keycode) {
  switch (keycode) {
  case SDLK_LSHIFT:
  case SDLK_RSHIFT:
    camera.MovementSpeed += 10;
    break;
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

glm::vec3 sdl2cpp::ui::CameraController::getPosition() const { return camera.Position; }

glm::mat4 sdl2cpp::ui::CameraController::getViewMatrix() { return camera.GetViewMatrix(); }

void sdl2cpp::ui::CameraController::draw([[maybe_unused]] sdl2cpp::ui::GUIRenderer &renderer) {}

void sdl2cpp::ui::CameraController::onMouseWheel([[maybe_unused]] ScrollDirection direction, int offset) {
  camera.ProcessMouseScroll(static_cast<float>(offset));
}
void sdl2cpp::ui::CameraController::onKeyUp(SDL_Keycode keycode) {
  switch (keycode) {
  case SDLK_LSHIFT:
  case SDLK_RSHIFT:
    camera.MovementSpeed -= 10;
    break;
  default:
    break;
  }
}
std::string sdl2cpp::ui::CameraController::info() const { return "camera controller"; }
void sdl2cpp::ui::CameraController::setMovementSpeed(float movementSpeed) { camera.MovementSpeed = movementSpeed; }
