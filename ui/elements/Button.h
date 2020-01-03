//
// Created by petr on 10/10/19.
//

#ifndef TERRAINGENERATION_BUTTON_H
#define TERRAINGENERATION_BUTTON_H

#include "ui/interface/MouseInteractable.h"
#include <memory>

namespace sdl2cpp::ui {

/**
 * Typical button with text.
 */
class Button : public MouseInteractable {
  OBSERVABLE_PROPERTIES(Button)

public:
  Button(UIManager &guiManager, glm::vec3 position, glm::vec3 dimensions);

  Text text;

  void setColor(glm::vec4 color);
  const glm::vec4 &getColor() const;
  std::string info() const override;

protected:
  void draw(GUIRenderer &renderer) override;
  void onEnabledChanged(bool enabled) override;
  void onMouseDown(MouseButton button, SDL_Point point) override;
  void onMouseUp(MouseButton button, SDL_Point point) override;
  void onMouseOver() override;
  void onMouseOut() override;

private:
  glm::vec4 color{1, 0, 0, 1};

  std::shared_ptr<ge::gl::Buffer> buffer;
  std::shared_ptr<ge::gl::VertexArray> vao;
};
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_BUTTON_H
