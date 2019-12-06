//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_UIMANAGER_H
#define TERRAINGENERATION_UIMANAGER_H

#include <ui/interface/ui_traits.h>
#include <utility>

#include "EventDispatcher.h"
#include "FocusManager.h"

namespace sdl2cpp::ui {
class UIManager {
public:
  explicit UIManager(const std::shared_ptr<Window>& window, const String &fontPath)
      : window(window), eventDispatcher(window, focusManager),
        renderer(fontPath) {}
  UIManager(const UIManager &) = delete;
  UIManager &operator=(const UIManager &) = delete;

  template <C_UIObject T, typename... Args>
  std::enable_if_t<std::is_constructible_v<T, UIManager &, Args...>,
      std::shared_ptr<T>>
  createGUIObject(Args &&... args);

  void render(glm::mat4 projection = glm::mat4(1),
              glm::mat4 view = glm::mat4(1), glm::mat4 model = glm::mat4(1));

  FontManager &getFontManager();
  FocusManager &getFocusManager();
  TextRenderer &getTextRenderer();

  template <C_UIObject T = UIObject>
  std::optional<std::shared_ptr<T>> objectByName(std::string_view name) {
    if (auto iter = std::find_if(objects.begin(), objects.end(),
                                 [name](const auto &obj) {
                                   if (obj.expired()) {
                                     return false;
                                   }
                                   auto ptr = obj.lock();
                                   if (ptr->getId() == name) {
                                     return true;
                                   }
                                   return false;
                                 });
        iter != objects.end()) {
      if (iter->expired()) {
        return std::nullopt;
      }
      auto ptr = iter->lock();
      if constexpr (std::is_same_v<T, UIObject>) {
        return ptr;
      } else {
        return std::dynamic_pointer_cast<T>(ptr);
      }
    }
    return std::nullopt;
  }

  void enqueueEvent(TimedEvent &&event) {
    eventDispatcher.addEvent(std::forward<TimedEvent&&>(event));
  }

private:
  std::shared_ptr<Window> window;
  std::vector<std::weak_ptr<UIVisible>> drawable;
  std::vector<std::weak_ptr<UIObject>> objects;
  EventDispatcher eventDispatcher;
  FocusManager focusManager{objects};

  GUIRenderer renderer;
};

template <C_UIObject T, typename... Args>
std::enable_if_t<std::is_constructible_v<T, UIManager &, Args...>,
    std::shared_ptr<T>>
UIManager::createGUIObject(Args &&... args) {
  auto result = std::make_shared<T>(*this, std::forward<Args>(args)...);
  if constexpr (is_mouse_interactable<T>) {
    eventDispatcher.addMouseEventListener(result);
  }
  if constexpr (is_keyboard_interactable<T>) {
    eventDispatcher.addKeyboardEventListener(result);
  }
  if constexpr (is_ui_drawable<T>) {
    drawable.emplace_back(result);
  }
  objects.emplace_back(result);
  return result;
}

} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_UIMANAGER_H
