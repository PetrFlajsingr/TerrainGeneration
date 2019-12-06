//
// Created by petr on 12/6/19.
//

#ifndef TERRAINGENERATION_SLIDER_H
#define TERRAINGENERATION_SLIDER_H

#include "ui/interface/MouseInteractable.h"

namespace sdl2cpp::ui {

template <typename T>
class Slider : public CustomMouseInteractable {
public:
  T getSliderValue() const;
  void setSliderValue(T sliderValue);
  T getMin() const;
  void setMin(T min);
  T getMax() const;
  void setMax(T max);
  T getStep() const;
  void setStep(T step);

protected:
  void onMouseDown(const SDL_Event &event) override;
  void onMouseUp(const SDL_Event &event) override;
  void onMouseMove(const SDL_Event &event) override;
  void onMouseClicked(const SDL_Event &event) override;
  void onMouseDblClicked(const SDL_Event &event) override;
  void onMouseOver(const SDL_Event &event) override;
  void onMouseOut(const SDL_Event &event) override;
  void onMouseWheel(const SDL_Event &event) override;

  void onFocusChanged(Focus focus) override;
  void onEnabledChanged(bool enabled) override;
  void draw(GUIRenderer &renderer) override;
  void onVisibilityChanged(Visibility visibility) override;

private:
  T sliderValue = T{0};
  T min = T{0};
  T max = T{100};
  T step = T{1};

  const float margin = 0.1f;
};

template <typename T> T sdl2cpp::ui::Slider<T>::getSliderValue() const {
  return sliderValue;
}
template <typename T>
void sdl2cpp::ui::Slider<T>::setSliderValue(T sliderValue) {
  Slider::sliderValue = sliderValue;
}
template <typename T> T sdl2cpp::ui::Slider<T>::getMin() const { return min; }
template <typename T> void sdl2cpp::ui::Slider<T>::setMin(T min) {
  Slider::min = min;
}
template <typename T> T sdl2cpp::ui::Slider<T>::getMax() const { return max; }
template <typename T> void sdl2cpp::ui::Slider<T>::setMax(T max) {
  Slider::max = max;
}
template <typename T> T sdl2cpp::ui::Slider<T>::getStep() const { return step; }
template <typename T> void sdl2cpp::ui::Slider<T>::setStep(T step) {
  Slider::step = step;
}

template <typename T> void Slider<T>::onMouseDown(const SDL_Event &event) {}
template <typename T> void Slider<T>::onMouseUp(const SDL_Event &event) {}
template <typename T> void Slider<T>::onMouseMove(const SDL_Event &event) {}
template <typename T> void Slider<T>::onMouseClicked(const SDL_Event &event) {}
template <typename T>
void Slider<T>::onMouseDblClicked(const SDL_Event &event) {}
template <typename T> void Slider<T>::onMouseOver(const SDL_Event &event) {}
template <typename T> void Slider<T>::onMouseOut(const SDL_Event &event) {}
template <typename T> void Slider<T>::onMouseWheel(const SDL_Event &event) {}

template <typename T> void Slider<T>::onFocusChanged(Focus focus) {}
template <typename T> void Slider<T>::onEnabledChanged(bool enabled) {}
template <typename T> void Slider<T>::draw(GUIRenderer &renderer) {}
template <typename T>
void Slider<T>::onVisibilityChanged(Visibility visibility) {}

}

#endif // TERRAINGENERATION_SLIDER_H
