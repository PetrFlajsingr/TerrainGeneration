//
// Created by petr on 12/6/19.
//

#ifndef TERRAINGENERATION_SLIDER_H
#define TERRAINGENERATION_SLIDER_H

#include "ui/interface/MouseInteractable.h"

namespace sdl2cpp::ui {

template <typename T> class Slider : public CustomEventMouseInteractable {
  OBSERVABLE_PROPERTIES(Slider)
public:
  using value_type = T;
  Slider(UIManager &guiManager, glm::vec3 position, glm::vec3 dimensions)
      : UIObject(guiManager), UIVisible(position, dimensions) {}

  template_observable_property<value_type> value;

  void setSliderValue(T sliderValue);
  T getMin() const;
  void setMin(T min);
  T getMax() const;
  void setMax(T max);
  T getStep() const;
  void setStep(T step);

  void step();

protected:
  void onFocusChanged(Focus focus) override;
  void draw(GUIRenderer &renderer) override;
  void onVisibilityChanged(Visibility visibility) override;

  void onMouseDown(EventInfo info, MouseButton button,
                   SDL_Point point) override;
  void onMouseUp(EventInfo info, MouseButton button, SDL_Point point) override;
  void onMouseMove(EventInfo info, SDL_Point newPos, SDL_Point oldPos) override;
  void onMouseClicked(EventInfo info, MouseButton button,
                      SDL_Point point) override;
  void onMouseDblClicked(EventInfo info, MouseButton button,
                         SDL_Point point) override;
  void onMouseOver(EventInfo info) override;
  void onMouseOut(EventInfo info) override;
  void onMouseWheel(EventInfo info, ScrollDirection direction, int i) override;

private:
  T min = T{0};
  T max = T{100};
  T sliderStep = T{1};
  T internalValue = T{0};

  const float margin = 0.1f;
};

template <typename T>
void sdl2cpp::ui::Slider<T>::setSliderValue(T sliderValue) {
  if (sliderValue > max) {
    value = max;
  } else if (sliderValue < min) {
    value = min;
  } else {
    value = sliderValue;
  }
}
template <typename T> T sdl2cpp::ui::Slider<T>::getMin() const { return min; }
template <typename T> void sdl2cpp::ui::Slider<T>::setMin(T min) {
  Slider::min = min;
}
template <typename T> T sdl2cpp::ui::Slider<T>::getMax() const { return max; }
template <typename T> void sdl2cpp::ui::Slider<T>::setMax(T max) {
  Slider::max = max;
}
template <typename T> T sdl2cpp::ui::Slider<T>::getStep() const {
  return sliderStep;
}
template <typename T> void sdl2cpp::ui::Slider<T>::setStep(T step) {
  Slider::sliderStep = step;
}

template <typename T> void Slider<T>::onFocusChanged(Focus focus) {}
template <typename T> void Slider<T>::draw(GUIRenderer &renderer) {}
template <typename T>
void Slider<T>::onVisibilityChanged(Visibility visibility) {}
template <typename T> void Slider<T>::step() {
  auto newValue = value.get() + sliderStep;
  setSliderValue(newValue);
}
template <typename T>
void Slider<T>::onMouseDown(EventInfo info, MouseButton button,
                            SDL_Point point) {}
template <typename T>
void Slider<T>::onMouseUp(EventInfo info, MouseButton button, SDL_Point point) {

}
template <typename T>
void Slider<T>::onMouseMove(EventInfo info, SDL_Point newPos,
                            SDL_Point oldPos) {
  if (getButtonState(MouseButton::Left) == MouseButtonState::Pressed) {
    setSliderValue(value.get() + (newPos.x - oldPos.x) * sliderStep);
  }
}
template <typename T>
void Slider<T>::onMouseClicked(EventInfo info, MouseButton button,
                               SDL_Point point) {}
template <typename T>
void Slider<T>::onMouseDblClicked(EventInfo info, MouseButton button,
                                  SDL_Point point) {}
template <typename T> void Slider<T>::onMouseOver(EventInfo info) {}
template <typename T> void Slider<T>::onMouseOut(EventInfo info) {}
template <typename T>
void Slider<T>::onMouseWheel(EventInfo info, ScrollDirection direction, int i) {

}

} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_SLIDER_H
