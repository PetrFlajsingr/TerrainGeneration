//
// Created by petr on 10/10/19.
//

#ifndef TERRAINGENERATION_BUTTON_H
#define TERRAINGENERATION_BUTTON_H
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>

#include <SDL.h>
#include <SDL2CPP/Window.h>
#include <geGL/Buffer.h>
#include <utility>

#include "ui/interface/MouseInteractable.h"
#include "ui/interface/UIObject.h"
#include "ui/ui_types.h"
#include <String.h>
#include <geGL_utils.h>
#include <glm/vec4.hpp>
#include <set>

namespace sdl2cpp::ui {

/**
 * Typical button with text.
 */
class Button : public MouseInteractable {
  OBSERVABLE_PROPERTIES(Button);

public:
  Button(UIManager &guiManager, glm::vec3 position, glm::vec3 dimensions);

  Text text;

protected:
  void draw(GUIRenderer &renderer) override;
  void onVisibilityChanged(Visibility visibility) override;
  void onFocusChanged(Focus focus) override;
  void onEnabledChanged(bool enabled) override;
  void onMouseDown(EventInfo info, MouseButton button,
                   SDL_Point point) override;
  void onMouseUp(EventInfo info, MouseButton button, SDL_Point point) override;
  void onMouseOver(EventInfo info) override;
  void onMouseOut(EventInfo info) override;

private:
  glm::vec4 color{1, 0, 0, 1};

  std::shared_ptr<ge::gl::Buffer> buffer;
  std::shared_ptr<ge::gl::VertexArray> vao;
};
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_BUTTON_H
