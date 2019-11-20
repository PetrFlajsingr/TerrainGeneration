//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_UIOBJECT_H
#define TERRAINGENERATION_UIOBJECT_H

#include "ui/GUIRenderer.h"
#include "ui/ui_types.h"
#include <SDL_render.h>
#include <SDL_surface.h>

namespace sdl2cpp::ui {

class UIObject {
  friend class FocusManager;
public:
  UIObject() = default;
  virtual ~UIObject() = default;

  UIObject(UIObject &&) = delete;
  UIObject &operator=(UIObject &&) = delete;

  void setEnabled(bool enabled);
  [[nodiscard]] bool isEnabled() const;
  [[nodiscard]] bool isDisabled() const;
  [[nodiscard]] bool isFocused() const;

protected:
  void setFocus(Focus focus);

  virtual void onFocusChanged(Focus focus) = 0;
  virtual void onEnabledChanged(bool enabled) = 0;

private:
  Visibility visiblity = Visibility::Visible;
  Focus focus = Focus::NotFocused;
  bool enabled = true;
};

class UIVisible : public virtual UIObject {
  friend class GUIManager;

public:
  explicit UIVisible(SDL_Rect area, int zPosition = 0);
  UIVisible(int x, int y, int width, int height, int zPosition = 0);

  UIVisible(UIVisible &&) = delete;
  UIVisible &operator=(UIVisible &&) = delete;
  void setVisibility(Visibility visibility);

  [[nodiscard]] SDL_Rect getArea() const;

  [[nodiscard]] bool isVisible() const;

  [[nodiscard]] int getZPosition() const;

protected:
  virtual void draw(GUIRenderer &renderer) = 0;
  virtual void onVisibilityChanged(Visibility visibility) = 0;

  SDL_Rect area;
private:
  Visibility visiblity = Visibility::Visible;

  int zPosition;
};

}

#endif // TERRAINGENERATION_UIOBJECT_H
