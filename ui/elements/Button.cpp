//
// Created by petr on 10/10/19.
//

#include "Button.h"
#include <FTGL/ftgl.h>
#include <GL/glut.h>
#include <SDL_events.h>
#include <geGL/StaticCalls.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <io/print.h>
#include <ui/utils.h>

sdl2cpp::ui::Button::Button(glm::vec3 position, glm::vec3 dimensions)
    : MouseInteractable(), UIVisible(position, dimensions) {
  SDL_Rect rect{static_cast<int>(position.x), static_cast<int>(position.y),
                static_cast<int>(dimensions.x), static_cast<int>(dimensions.y)};
  auto positions = sdlRectToGLCoordinates(rect, 1920, 1080);
  buffer = createBuffer<glm::vec3>(4, GL_STATIC_DRAW, &positions[0]);
  vao = std::make_shared<ge::gl::VertexArray>();
  vao->addAttrib(buffer, 0, 3, GL_FLOAT, sizeof(float) * 3, 0, GL_FALSE);

  text.subscribe([this](const WString &newText) { newTextSet = true; });
}
void sdl2cpp::ui::Button::draw(GUIRenderer &renderer) {
  renderer.getProgram()->set4fv("color", &color[0]);
  vao->bind();
  ge::gl::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  vao->unbind();

  if (newTextSet) {
    glm::vec3 pen{0, 1000, 0};
    glm::vec4 black = {0, 0, 1, 1};
    t.setText(
        renderer.getTextRenderer().getFontManager().getFont("arialbd"_s, 100),
        text.get(), black, pen);
    newTextSet = false;
  }
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
void sdl2cpp::ui::Button::setText(const WString &text) { this->text.set(text); }
