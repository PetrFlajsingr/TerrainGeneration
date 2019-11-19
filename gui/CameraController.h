//
// Created by petr on 10/20/19.
//

#ifndef TERRAINGENERATION_CAMERACONTROLLER_H
#define TERRAINGENERATION_CAMERACONTROLLER_H

#include "UserInteraction.h"
#include <Camera.h>
#include <SDL_events.h>
#include <functional>

class CameraController : public sdl2cpp::gui::MouseInteractable, public sdl2cpp::gui::KeyboardInteractable {
public:
  explicit CameraController(SDL_Rect area, glm::vec3 startingPosition = {0.f, 0.f, 0.f},
      glm::vec3 direction = {0.f, 0.f, -1.f});

  [[nodiscard]] glm::vec3 getPosition() const;
  [[nodiscard]] glm::mat4 getViewMatrix();
  Camera camera;

protected:
  void onMouseDown(const SDL_Event &event) override;
  void draw() override;
  void onVisibilityChanged(sdl2cpp::gui::Visibility visibility) override;
  void onFocusChanged(sdl2cpp::gui::Focus focus) override;
  void onMouseClicked(const SDL_Event &event) override;
  void onMouseDblClicked(const SDL_Event &event) override;
  void onMouseOver(const SDL_Event &event) override;
  void onKeyUp(const SDL_Event &event) override;
  void onMouseOut(const SDL_Event &event) override;
  void onEnabledChanged(bool enabled) override;
  void onMouseUp(const SDL_Event &event) override;
  void onMouseMove(const SDL_Event &event) override;
  void onKeyPressed(const SDL_Event &event) override;
  void onKeyDown(const SDL_Event &event) override;

private:
  bool lockedToCamera = false;
};

#endif // TERRAINGENERATION_CAMERACONTROLLER_H
