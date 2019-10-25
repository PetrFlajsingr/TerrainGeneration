//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_GUIOBJECT_H
#define TERRAINGENERATION_GUIOBJECT_H

#include "Types.h"
#include <SDL_surface.h>
namespace sdl2cpp::gui {

class GUIObject {
  friend class FocusManager;
public:
  explicit GUIObject(SDL_Rect area);
  GUIObject(int x, int y, int width, int height);
  virtual ~GUIObject() = default;

  GUIObject(GUIObject &&) = delete;
  GUIObject &operator=(GUIObject &&) = delete;

  void setVisibility(Visibility visibility);
  void setEnabled(bool enabled);
  [[nodiscard]] bool isVisible() const;
  [[nodiscard]] bool isEnabled() const;
  [[nodiscard]] bool isDisabled() const;
  [[nodiscard]] bool isFocused() const;

  [[nodiscard]] SDL_Rect getArea() const;

protected:
  void setFocus(Focus focus);

  virtual void draw() = 0;
  virtual void onVisibilityChanged(Visibility visibility) = 0;
  virtual void onFocusChanged(Focus focus) = 0;
  virtual void onEnabledChanged(bool enabled) = 0;

  SDL_Rect area;

private:
  Visibility visiblity = Visibility::Visible;
  Focus focus = Focus::NotFocused;
  bool enabled = true;
};

}

#endif // TERRAINGENERATION_GUIOBJECT_H
