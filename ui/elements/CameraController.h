//
// Created by petr on 10/20/19.
//

#ifndef TERRAINGENERATION_CAMERACONTROLLER_H
#define TERRAINGENERATION_CAMERACONTROLLER_H

#include "ui/interface/KeyboardInteractable.h"
#include "ui/interface/MouseInteractable.h"
#include <Camera.h>
#include <SDL_events.h>
#include <functional>

namespace sdl2cpp::ui {
/**
 * Camera controls. Steals mouse on right click and accepts other events while
 * the right mouse button is pressed.
 */
class CameraController : public sdl2cpp::ui::CustomEventMouseInteractable, public sdl2cpp::ui::CustomEventKeyboardInteractable {
public:
  explicit CameraController(UIManager &guiManager, PerspectiveProjection projection, glm::vec3 position, glm::vec3 dimensions,
                            glm::vec3 startingPosition = {0.f, 0.f, 0.f}, glm::vec3 direction = {0.f, 0.f, -1.f});

  /**
   * @return camera position
   */
  [[nodiscard]] glm::vec3 getPosition() const;
  [[nodiscard]] glm::mat4 getViewMatrix();

  void setMovementSpeed(float movementSpeed);

  Camera camera;
  std::string info() const override;

protected:
  void onMouseWheel(EventInfo info, ScrollDirection direction, int offset) override;
  void onMouseDown(EventInfo info, MouseButton button, SDL_Point position) override;
  void draw(sdl2cpp::ui::GUIRenderer &renderer) override;

  void onMouseUp(EventInfo info, MouseButton button, SDL_Point position) override;
  void onMouseMove(EventInfo info, SDL_Point newPos, SDL_Point oldPos) override;
  void onKeyDown(SDL_Keycode keycode) override;
  void onKeyUp(SDL_Keycode keycode) override;

private:
  bool lockedToCamera = false;
};
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_CAMERACONTROLLER_H
