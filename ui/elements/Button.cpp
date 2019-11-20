//
// Created by petr on 10/10/19.
//

#include "Button.h"
#include <SDL_events.h>
#include <geGL/StaticCalls.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <ui/utils.h>

sdl2cpp::ui::Button::Button(int x, int y, int w, int h, int zPosition)
    : CustomMouseInteractable(), UIVisible(x, y, w, h, zPosition) {
  box.x = x;
  box.y = y;
  box.w = w;
  box.h = h;
  auto positions = sdlRectToGLCoordinates(getArea(), 1920, 1080);
  buffer = createBuffer<glm::vec3>(4, GL_STATIC_DRAW, &positions[0]);
  vao = std::make_shared<ge::gl::VertexArray>();
  vao->addAttrib(buffer, 0, 3, GL_FLOAT, sizeof(float) * 3, 0, GL_FALSE);
}
void sdl2cpp::ui::Button::draw(GUIRenderer &renderer) {
  renderer.getProgram()->set4fv("color", &color[0]);
  vao->bind();
  ge::gl::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glm::vec2 textPosition{10, 10};
  glm::vec4 black = {0, 0, 0, 1};
}

void sdl2cpp::ui::Button::onVisibilityChanged(Visibility visibility) {}

void sdl2cpp::ui::Button::onFocusChanged(Focus focus) {}

void sdl2cpp::ui::Button::onEnabledChanged(bool enabled) {}
void sdl2cpp::ui::Button::onMouseOver(const SDL_Event &event) {
  MouseInteractable::onMouseOver(event);
  color = {0, 1, 0, 1};
}
void sdl2cpp::ui::Button::onMouseOut(const SDL_Event &event) {
  MouseInteractable::onMouseOut(event);
  color = {1, 0, 0, 1};
}
void sdl2cpp::ui::Button::onMouseDown(const SDL_Event &event) {
  MouseInteractable::onMouseDown(event);
  color = {0, 0, 1, 1};
}
void sdl2cpp::ui::Button::onMouseUp(const SDL_Event &event) {
  MouseInteractable::onMouseUp(event);
  color = {1, 0, 0, 1};
}
