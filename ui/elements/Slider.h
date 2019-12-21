//
// Created by petr on 12/6/19.
//

#ifndef TERRAINGENERATION_SLIDER_H
#define TERRAINGENERATION_SLIDER_H

#include "ui/interface/MouseInteractable.h"
#include <graphics/geGL_utils.h>
#include <ui/utils.h>

namespace sdl2cpp::ui {

template <typename T> class Slider : public CustomEventMouseInteractable {
  OBSERVABLE_PROPERTIES(Slider)
public:
  using value_type = T;
  Slider(UIManager &guiManager, glm::vec3 position, glm::vec3 dimensions)
      : UIObject(guiManager), UIVisible(position, dimensions) {
    SDL_Rect rect{static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(dimensions.x),
                  static_cast<int>(dimensions.y)};
    auto positions = sdlRectToGLCoordinates(rect, 1920, 1080);
    buffer = createBuffer<glm::vec3>(4, GL_STATIC_DRAW, &positions[0]);
    vao = std::make_shared<ge::gl::VertexArray>();
    vao->addAttrib(buffer, 0, 3, GL_FLOAT, sizeof(float) * 3, 0, GL_FALSE);
  }

  template_observable_property<value_type> value;

  Slider &setSliderValue(T sliderValue);
  T getMin() const;
  Slider &setMin(T min);
  T getMax() const;
  Slider &setMax(T max);
  T getStep() const;
  Slider &setStep(T step);

  void setColor(const glm::vec4 &color);

  void step();
  std::string info() const override;

protected:
  void draw(GUIRenderer &renderer) override;

  void onMouseDown(EventInfo info, MouseButton button, SDL_Point point) override;
  void onMouseUp(EventInfo info, MouseButton button, SDL_Point point) override;
  void onMouseMove(EventInfo info, SDL_Point newPos, SDL_Point oldPos) override;
  void onMouseOver(EventInfo info) override;
  void onMouseOut(EventInfo info) override;

private:
  T min = T{0};
  T max = T{100};
  T sliderStep = T{1};
  T internalValue = T{0};

  const float margin = 0.1f;

  glm::vec4 color{1, 0, 0, 1};

  std::shared_ptr<ge::gl::Buffer> buffer;
  std::shared_ptr<ge::gl::VertexArray> vao;
};

template <typename T> sdl2cpp::ui::Slider<T> &sdl2cpp::ui::Slider<T>::setSliderValue(T sliderValue) {
  if (sliderValue > max) {
    value = max;
  } else if (sliderValue < min) {
    value = min;
  } else {
    value = sliderValue;
  }
  return *this;
}

template <typename T> T sdl2cpp::ui::Slider<T>::getMin() const { return min; }
template <typename T> sdl2cpp::ui::Slider<T> &sdl2cpp::ui::Slider<T>::setMin(T min) {
  Slider::min = min;
  return *this;
}

template <typename T> T sdl2cpp::ui::Slider<T>::getMax() const { return max; }

template <typename T> sdl2cpp::ui::Slider<T> &sdl2cpp::ui::Slider<T>::setMax(T max) {
  Slider::max = max;
  return *this;
}

template <typename T> T sdl2cpp::ui::Slider<T>::getStep() const { return sliderStep; }

template <typename T> sdl2cpp::ui::Slider<T> &sdl2cpp::ui::Slider<T>::setStep(T step) {
  Slider::sliderStep = step;
  return *this;
}

template <typename T> void Slider<T>::draw(GUIRenderer &renderer) {
  const auto valueRange = max - min;
  const auto percentage = (valueRange - value.get()) / valueRange;
  const auto width = dimensions.get().x * (1.f - percentage);
  const auto position = this->position.get();
  const auto dimensions = this->dimensions.get();
  SDL_Rect rect{static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(width),
                static_cast<int>(dimensions.y)};
  auto positions = sdlRectToGLCoordinates(rect, 1920, 1080);
  buffer->setData(positions.data());

  renderer.getProgram()->set4fv("color", &color[0]);
  vao->bind();
  ge::gl::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  vao->unbind();
}

template <typename T> void Slider<T>::step() {
  auto newValue = value.get() + sliderStep;
  setSliderValue(newValue);
}
template <typename T> void Slider<T>::onMouseDown(EventInfo info, MouseButton button, SDL_Point point) {
  if (getButtonState(MouseButton::Left) == MouseButtonState::Pressed) {
    const auto sliderWidth = dimensions.get().x;
    const auto percentageTraveled = (point.x - position.get().x) / sliderWidth * 2;
    const auto valueDelta = percentageTraveled * 100 * sliderStep;
    setSliderValue( valueDelta);
  }
}
template <typename T> void Slider<T>::onMouseUp(EventInfo info, MouseButton button, SDL_Point point) {}
template <typename T> void Slider<T>::onMouseMove(EventInfo info, SDL_Point newPos, SDL_Point oldPos) {
  if (getButtonState(MouseButton::Left) == MouseButtonState::Pressed) {
    const auto sliderWidth = dimensions.get().x;
    const auto percentageTraveled = (newPos.x - oldPos.x) / sliderWidth * 2;
    const auto valueDelta = percentageTraveled * 100 * sliderStep;
    setSliderValue(value.get() + valueDelta);
  }
}

template <typename T> void Slider<T>::onMouseOver(EventInfo info) {
  setColor({0, 1, 0, 1});
}
template <typename T> void Slider<T>::onMouseOut(EventInfo info) {
  setColor({1, 0, 0, 1});
}
template <typename T> std::string Slider<T>::info() const { return "Slider"; }
template <typename T> void Slider<T>::setColor(const glm::vec4 &color) { Slider::color = color; }

} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_SLIDER_H
