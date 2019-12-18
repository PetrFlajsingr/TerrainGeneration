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
  SDL_Rect rect{static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(dimensions.x),
                static_cast<int>(dimensions.y)};
  auto positions = sdlRectToGLCoordinates(rect, 1920, 1080);
  buffer = createBuffer<glm::vec3>(4, GL_STATIC_DRAW, &positions[0]);
  vao = std::make_shared<ge::gl::VertexArray>();
  vao->addAttrib(buffer, 0, 3, GL_FLOAT, sizeof(float) * 3, 0, GL_FALSE);

  text.setPosition({position.x / 1920 * 1000, 562.5 - (position.y + dimensions.y) / 1080 * 562.5, 0});
  text.setColor({0, 0, 1, 1});
}
void sdl2cpp::ui::Button::draw(GUIRenderer &renderer) {
  renderer.getProgram()->set4fv("color", &color[0]);
  vao->bind();
  ge::gl::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  vao->unbind();
}

void sdl2cpp::ui::Button::onVisibilityChanged(Visibility visibility) {}

void sdl2cpp::ui::Button::onFocusChanged(Focus focus) {}

void sdl2cpp::ui::Button::onEnabledChanged(bool enabled) {
  Interactable::onEnabledChanged(enabled);
  if (!enabled) {
    color = glm::vec4{0.5, 0.5, 0.5, 1};
  } else {
    color = glm::vec4{1, 0, 0, 1};
  }
}

void sdl2cpp::ui::Button::onMouseDown(sdl2cpp::ui::EventInfo info, sdl2cpp::ui::MouseButton button, SDL_Point point) {
  MouseInteractable::onMouseDown(info, button, point);
  color = {0, 0, 1, 1};
}

void sdl2cpp::ui::Button::onMouseUp(sdl2cpp::ui::EventInfo info, sdl2cpp::ui::MouseButton button, SDL_Point point) {
  MouseInteractable::onMouseUp(info, button, point);
  color = {1, 0, 0, 1};
}

void sdl2cpp::ui::Button::onMouseOver(sdl2cpp::ui::EventInfo info) {
  MouseInteractable::onMouseOver(info);
  color = {0, 1, 0, 1};
}

void sdl2cpp::ui::Button::onMouseOut(sdl2cpp::ui::EventInfo info) {
  MouseInteractable::onMouseOut(info);
  color = {1, 0, 0, 1};
}
void sdl2cpp::ui::Button::setColor(glm::vec4 color) { Button::color = color; }
const glm::vec4 &sdl2cpp::ui::Button::getColor() const { return color; }
