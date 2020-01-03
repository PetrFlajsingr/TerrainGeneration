//
// Created by petr on 12/24/19.
//

#ifndef TERRAINGENERATION_SWITCH_H
#define TERRAINGENERATION_SWITCH_H

#include <Colors.h>
#include <geGL/Buffer.h>
#include <geGL/VertexArray.h>
#include <ui/interface/MouseInteractable.h>

class Switch : public sdl2cpp::ui::CustomEventMouseInteractable {
  OBSERVABLE_PROPERTIES(Switch)
public:
  Switch(sdl2cpp::ui::UIManager &uiManager, glm::vec3 position, glm::vec3 dimensions, bool isOn);

  observable_property<bool> isOn;

  std::string info() const override;

protected:
  void onMouseClicked(sdl2cpp::ui::MouseButton button, glm::vec2 point) override;

protected:
  void draw(sdl2cpp::ui::GUIRenderer &renderer) override;

private:
  std::shared_ptr<ge::gl::Buffer> buffer;
  std::shared_ptr<ge::gl::VertexArray> vao;

  glm::vec4 colorForState();
};

#endif // TERRAINGENERATION_SWITCH_H
