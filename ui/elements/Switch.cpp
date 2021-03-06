//
// Created by petr on 12/24/19.
//

#include "Switch.h"
#include "ui/managers/UIManager.h"
#include <graphics/geGL_utils.h>
#include <ui/utils.h>

Switch::Switch(sdl2cpp::ui::UIManager &uiManager, glm::vec3 position, glm::vec3 dimensions, bool isOn)
    : UIObject(uiManager), UIVisible(position, dimensions), isOn(isOn) {
  const auto [windowWidth, windowHeight] = uiManager.getWindowSize();
  SDL_Rect rect{static_cast<int>(position.x * windowWidth), static_cast<int>(position.y * windowHeight),
                static_cast<int>(dimensions.x * windowWidth), static_cast<int>(dimensions.y * windowHeight)};
  auto positions = sdlRectToGLCoordinates(rect, windowWidth, windowHeight);
  buffer = createBuffer<glm::vec3>(4, GL_STATIC_DRAW, &positions[0]);
  vao = std::make_shared<ge::gl::VertexArray>();
  vao->addAttrib(buffer, 0, 3, GL_FLOAT, sizeof(float) * 3, 0, GL_FALSE);
}

std::string Switch::info() const { return "Switch"; }

void Switch::draw(sdl2cpp::ui::GUIRenderer &renderer) {
  renderer.getProgram()->set4fv("color", &colorForState()[0]);
  vao->bind();
  ge::gl::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  vao->unbind();
}

void Switch::onMouseClicked([[maybe_unused]] sdl2cpp::ui::MouseButton button, [[maybe_unused]] glm::vec2 point) {
  isOn = !isOn.get();
}

glm::vec4 Switch::colorForState() {
  if (isOn) {
    return Color::green;
  } else {
    return Color::red;
  }
}
