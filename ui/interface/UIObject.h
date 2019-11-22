//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_UIOBJECT_H
#define TERRAINGENERATION_UIOBJECT_H

#include "observable/observable.hpp"
#include "ui/GUIRenderer.h"
#include "ui/ui_types.h"
#include <glm/vec3.hpp>

namespace sdl2cpp::ui {

/**
 * Common class for all ui objects.
 */
class UIObject {
  OBSERVABLE_PROPERTIES(UIObject);

public:
  UIObject() = default;
  virtual ~UIObject() = default;

  /**
   * When a component is inabled it accepts events, but it doesn't propagate
   * them into its behavior.
   * @param enabled
   */
  void setEnabled(bool enabled);
  /**
   * Focused component accepts keyboard events.
   * @param focus
   */
  void setFocus(Focus focus);

  observable_property<bool> enabled{true};
  observable_property<Focus> focus{Focus::NotFocused};

protected:
  virtual void onFocusChanged(Focus focus) = 0;
  virtual void onEnabledChanged(bool enabled) = 0;
};

/**
 * Common class for all ui objects which should be drawn to the screen.
 */
class UIVisible : public virtual UIObject {
  friend class GUIManager;
  OBSERVABLE_PROPERTIES(UIVisible);

public:
  UIVisible(glm::vec3 position, glm::vec3 dimensions);

  /**
   * When a component is invisible it is not drawn and it doesn't accept any
   * events.
   */
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

} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_UIOBJECT_H
