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

class Button : public MouseInteractable {
  OBSERVABLE_PROPERTIES(Button);

public:
  Button(glm::vec3 position, glm::vec3 dimensions);

  Text text;

protected:
  void draw(GUIRenderer &renderer) override;
  void onVisibilityChanged(Visibility visibility) override;
  void onFocusChanged(Focus focus) override;
  void onEnabledChanged(bool enabled) override;
  void onMouseOver(const SDL_Event &event) override;
  void onMouseOut(const SDL_Event &event) override;
  void onMouseDown(const SDL_Event &event) override;
  void onMouseUp(const SDL_Event &event) override;

private:
  glm::vec4 color{1, 0, 0, 1};

  std::shared_ptr<ge::gl::Buffer> buffer;
  std::shared_ptr<ge::gl::VertexArray> vao;
};
}

#endif // TERRAINGENERATION_BUTTON_H
