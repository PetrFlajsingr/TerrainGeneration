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
class CameraController : public sdl2cpp::ui::CustomMouseInteractable, public sdl2cpp::ui::CustomKeyboardInteractable {
public:
  explicit CameraController(UIManager &guiManager, PerspectiveProjection projection, glm::vec3 position, glm::vec3 dimensions,
                            glm::vec3 startingPosition = {0.f, 0.f, 0.f}, glm::vec3 direction = {0.f, 0.f, -1.f});

  /**
   * @return camera position
   */
  [[nodiscard]] glm::vec3 getPosition() const;
  [[nodiscard]] glm::mat4 getViewMatrix();
  Camera camera;

protected:
  void onMouseWheel(const SDL_Event &event) override;
  void onMouseDown(const SDL_Event &event) override;
  void draw(sdl2cpp::ui::GUIRenderer &renderer) override;
  void onVisibilityChanged(sdl2cpp::ui::Visibility visibility) override;
  void onFocusChanged(sdl2cpp::ui::Focus focus) override;

  void onEnabledChanged(bool enabled) override;
  void onMouseUp(const SDL_Event &event) override;
  void onMouseMove(const SDL_Event &event) override;
  void onKeyDown(const SDL_Event &event) override;
  void onKeyUp(const SDL_Event &event) override;

private:
  bool lockedToCamera = false;
};
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_CAMERACONTROLLER_H
