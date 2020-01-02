//
// Created by petr on 12/24/19.
//

#include "Switch.h"
#include <graphics/geGL_utils.h>
#include <ui/utils.h>
Switch::Switch(sdl2cpp::ui::UIManager &uiManager, glm::vec3 position, glm::vec3 dimensions, bool isOn)
    : UIObject(uiManager), UIVisible(position, dimensions), isOn(isOn) {
  SDL_Rect rect{static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(dimensions.x),
                static_cast<int>(dimensions.y)};
  auto positions = sdlRectToGLCoordinates(rect, 1920, 1080);
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
void Switch::onMouseClicked(sdl2cpp::ui::EventInfo info, sdl2cpp::ui::MouseButton button, SDL_Point point) { isOn = !isOn.get(); }
glm::vec4 Switch::colorForState() {
  if (isOn) {
    return Color::green;
  } else {
    return Color::red;
  }
}
