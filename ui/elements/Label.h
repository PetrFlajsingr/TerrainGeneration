//
// Created by petr on 11/23/19.
//

#ifndef TERRAINGENERATION_LABEL_H
#define TERRAINGENERATION_LABEL_H

#include <ui/interface/MouseInteractable.h>

namespace sdl2cpp::ui {

class Label : public UIVisible {
public:
  Label(UIManager &guiManager, glm::vec3 position, glm::vec3 dimensions);

  Text text;
  std::string info() const override;

protected:
  void draw(GUIRenderer &renderer) override;
};
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_LABEL_H
