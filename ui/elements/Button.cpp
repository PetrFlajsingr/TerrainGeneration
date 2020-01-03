//
// Created by petr on 10/10/19.
//

#include "Button.h"
#include "graphics/geGL_utils.h"
#include "ui/managers/UIManager.h"
#include <GL/glut.h>
#include <SDL_events.h>
#include <geGL/StaticCalls.h>
#include <io/print.h>
#include <ui/utils.h>

sdl2cpp::ui::Button::Button(UIManager &guiManager, glm::vec3 position, glm::vec3 dimensions)
    : UIObject(guiManager), UIVisible(position, dimensions), MouseInteractable(),
      text(guiManager.getTextRenderer().createText()) {
  const auto [windowWidth, windowHeight] = guiManager.getWindowSize();
  SDL_Rect rect{static_cast<int>(position.x * windowWidth), static_cast<int>(position.y * windowHeight),
                static_cast<int>(dimensions.x * windowWidth), static_cast<int>(dimensions.y * windowHeight)};
  auto positions = sdlRectToGLCoordinates(rect, windowWidth, windowHeight);
  buffer = createBuffer<glm::vec3>(4, GL_STATIC_DRAW, &positions[0]);
  vao = std::make_shared<ge::gl::VertexArray>();
  vao->addAttrib(buffer, 0, 3, GL_FLOAT, sizeof(float) * 3, 0, GL_FALSE);

  text.setPosition({position.x * 1000, 562.5 - (position.y + dimensions.y) * 562.5, 0});
  text.setColor({0, 0, 1, 1});
}

void sdl2cpp::ui::Button::draw(GUIRenderer &renderer) {
  renderer.getProgram()->set4fv("color", &color[0]);
  vao->bind();
  ge::gl::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  vao->unbind();
}

void sdl2cpp::ui::Button::onEnabledChanged(bool enabled) {
  Interactable::onEnabledChanged(enabled);
  if (!enabled) {
    color = glm::vec4{0.5, 0.5, 0.5, 1};
  } else {
    color = glm::vec4{1, 0, 0, 1};
  }
}

void sdl2cpp::ui::Button::onMouseDown(MouseButton button, glm::vec2 point) {
  MouseInteractable::onMouseDown(button, point);
  color = {0, 0, 1, 1};
}

void sdl2cpp::ui::Button::onMouseUp(MouseButton button, glm::vec2 point) {
  MouseInteractable::onMouseUp(button, point);
  color = {1, 0, 0, 1};
}

void sdl2cpp::ui::Button::onMouseOver() {
  MouseInteractable::onMouseOver();
  color = {0, 1, 0, 1};
}

void sdl2cpp::ui::Button::onMouseOut() {
  MouseInteractable::onMouseOut();
  color = {1, 0, 0, 1};
}
void sdl2cpp::ui::Button::setColor(glm::vec4 color) { Button::color = color; }
const glm::vec4 &sdl2cpp::ui::Button::getColor() const { return color; }
std::string sdl2cpp::ui::Button::info() const { return "Button"; }
