//
// Created by petr on 12/6/19.
//

#ifndef TERRAINGENERATION_SLIDER_H
#define TERRAINGENERATION_SLIDER_H

#include "ui/interface/MouseInteractable.h"
#include "ui/managers/UIManager.h"
#include <graphics/geGL_utils.h>
#include <io/print.h>
#include <ui/utils.h>

namespace sdl2cpp::ui {
#define template_observable_property typename ::observable::detail::prop_<Observable_Property_EnclosingType_>::template type

template <typename T> class Slider : public CustomEventMouseInteractable {
  OBSERVABLE_PROPERTIES(Slider)
public:
  using value_type = T;
  Slider(UIManager &guiManager, glm::vec3 position, glm::vec3 dimensions)
      : UIObject(guiManager), UIVisible(position, dimensions) {
    const auto [windowWidth, windowHeight] = guiManager.getWindowSize();
    SDL_Rect rect{static_cast<int>(position.x * windowWidth), static_cast<int>(position.y * windowHeight),
                  static_cast<int>(dimensions.x * windowWidth), static_cast<int>(dimensions.y * windowHeight)};
    auto positions = sdlRectToGLCoordinates(rect, windowWidth, windowHeight);
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

  void onMouseDown(MouseButton button, glm::vec2 point) override;
  void onMouseMove(glm::vec2 newPos, glm::vec2 oldPos) override;
  void onMouseOver() override;
  void onMouseOut() override;

private:
  T min = T{0};
  T max = T{100};
  double sliderStep = T{1};
  double internalValue = T{0};

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
  internalValue = value.get();
  return *this;
}

template <typename T> T sdl2cpp::ui::Slider<T>::getMin() const { return min; }
template <typename T> sdl2cpp::ui::Slider<T> &sdl2cpp::ui::Slider<T>::setMin(T min) {
  Slider::sliderStep = (max - min) / 100.0;
  Slider::min = min;
  return *this;
}

template <typename T> T sdl2cpp::ui::Slider<T>::getMax() const { return max; }

template <typename T> sdl2cpp::ui::Slider<T> &sdl2cpp::ui::Slider<T>::setMax(T max) {
  Slider::sliderStep = (max - min) / 100.0;
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
  const auto percentage = (valueRange - value.get() + min) / static_cast<double>(valueRange);
  const auto width = dimensions.get().x * (1.f - percentage);
  const auto position = this->position.get();
  const auto dimensions = this->dimensions.get();
  const auto [windowWidth, windowHeight] = getUIManager().getWindowSize();
  SDL_Rect rect{static_cast<int>(position.x * windowWidth), static_cast<int>(position.y * windowHeight),
                static_cast<int>(width * windowWidth), static_cast<int>(dimensions.y * windowHeight)};
  auto positions = sdlRectToGLCoordinates(rect, windowWidth, windowHeight);
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
template <typename T> void Slider<T>::onMouseDown(MouseButton button, glm::vec2 point) {
  if (button == MouseButton::Left) {
    const auto sliderWidth = dimensions.get().x;
    const auto percentageTraveled = (point.x - position.get().x) / sliderWidth;
    const auto valueDelta = min + percentageTraveled * 100 * sliderStep;
    internalValue = valueDelta;
    setSliderValue(internalValue);
  }
}

template <typename T> void Slider<T>::onMouseMove(glm::vec2 newPos, [[maybe_unused]] glm::vec2 oldPos) {
  if (getButtonState(MouseButton::Left) == MouseButtonState::Pressed) {
    const auto sliderWidth = dimensions.get().x;
    const auto percentageTraveled = (newPos.x - position.get().x) / sliderWidth;
    const auto valueDelta = min + percentageTraveled * 100 * sliderStep;
    internalValue = valueDelta;
    setSliderValue(internalValue);
  }
}
template <typename T> void Slider<T>::onMouseOver() { setColor({0, 1, 0, 1}); }
template <typename T> void Slider<T>::onMouseOut() { setColor({1, 0, 0, 1}); }
template <typename T> std::string Slider<T>::info() const { return "Slider"; }
template <typename T> void Slider<T>::setColor(const glm::vec4 &color) { Slider::color = color; }

} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_SLIDER_H
