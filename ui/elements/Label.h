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

protected:
  void onFocusChanged(Focus focus) override;
  void onEnabledChanged(bool enabled) override;
  void draw(GUIRenderer &renderer) override;
  void onVisibilityChanged(Visibility visibility) override;
};
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_LABEL_H
