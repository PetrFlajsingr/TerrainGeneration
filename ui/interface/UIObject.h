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
class UIObject : public std::enable_shared_from_this<UIObject> {
  OBSERVABLE_PROPERTIES(UIObject);

public:
  explicit UIObject(UIManager &guiManager);
  virtual ~UIObject() = default;

  /**
   * When a component is enabled it accepts events, but it doesn't propagate
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

  UIManager &getGUIManager();

  std::string_view getId() const;

  virtual std::string info() const = 0;

protected:
  virtual void onFocusChanged(Focus focus);
  virtual void onEnabledChanged(bool enabled);

private:
  UIManager &guiManager;
  std::string id;
};

/**
 * Common class for all ui objects which should be drawn to the screen.
 */
class UIVisible : public virtual UIObject {
  friend class UIManager;
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
  virtual void onVisibilityChanged(Visibility visibility);
};

} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_UIOBJECT_H
