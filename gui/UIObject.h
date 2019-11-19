//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_UIOBJECT_H
#define TERRAINGENERATION_UIOBJECT_H

#include "Types.h"
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

  virtual void draw() = 0;
  virtual void onFocusChanged(Focus focus) = 0;
  virtual void onEnabledChanged(bool enabled) = 0;

private:
  Visibility visiblity = Visibility::Visible;
  Focus focus = Focus::NotFocused;
  bool enabled = true;
};

class UIVisible : public virtual UIObject {
public:
  explicit UIVisible(SDL_Rect area);
  UIVisible(int x, int y, int width, int height);

  UIVisible(UIVisible &&) = delete;
  UIVisible &operator=(UIVisible &&) = delete;
  void setVisibility(Visibility visibility);

  [[nodiscard]] SDL_Rect getArea() const;

  [[nodiscard]] bool isVisible() const;

protected:
  virtual void draw() = 0;
  virtual void onVisibilityChanged(Visibility visibility) = 0;

  SDL_Rect area;
private:
  Visibility visiblity = Visibility::Visible;
};

}

#endif // TERRAINGENERATION_UIOBJECT_H
