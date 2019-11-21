//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_UIOBJECT_H
#define TERRAINGENERATION_UIOBJECT_H

#include "observable/observable.hpp"
#include "ui/GUIRenderer.h"
#include "ui/ui_types.h"
#include <SDL_render.h>
#include <SDL_surface.h>
#include <glm/vec3.hpp>

namespace sdl2cpp::ui {

class UIObject {
  friend class FocusManager;
  OBSERVABLE_PROPERTIES(UIObject);
public:
  UIObject() = default;
  virtual ~UIObject() = default;

  void setEnabled(bool enabled);

  observable_property<bool> enabled;
  observable_property<Focus> focus;

protected:
  void setFocus(Focus focus);

  virtual void onFocusChanged(Focus focus) = 0;
  virtual void onEnabledChanged(bool enabled) = 0;
};

class UIVisible : public virtual UIObject {
  friend class GUIManager;
  OBSERVABLE_PROPERTIES(UIVisible);
public:
  UIVisible(glm::vec3 position, glm::vec3 dimensions);

  void setVisibility(Visibility visibility);

  void setPosition(glm::vec3 position);

  void setDimensions(glm::vec3 dimensions);

  observable_property<Visibility> visibility;
  observable_property<glm::vec3> position;
  observable_property<glm::vec3> dimensions;

protected:
  virtual void draw(GUIRenderer &renderer) = 0;
  virtual void onVisibilityChanged(Visibility visibility) = 0;
};

}

#endif // TERRAINGENERATION_UIOBJECT_H
