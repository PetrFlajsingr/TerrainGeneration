//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_GUIMANAGER_H
#define TERRAINGENERATION_GUIMANAGER_H

#include <utility>

#include "EventDispatcher.h"
#include "FocusManager.h"

namespace sdl2cpp::ui {
class GUIManager {
public:
  explicit GUIManager(std::shared_ptr<Window> window, const String &fontPath)
      : window(std::move(window)), eventDispatcher(window), renderer(fontPath) {
  }
  GUIManager(const GUIManager &) = delete;
  GUIManager &operator=(const GUIManager &) = delete;

  template <typename T, typename... Args>
  [[nodiscard]] std::enable_if_t<std::is_base_of_v<UIObject, T> &&
                                     std::is_constructible_v<T, Args...>,
                                 std::shared_ptr<T>>
  createGUIObject(Args &&... args) {
    auto result = std::make_shared<T>(std::forward<Args>(args)...);
    if constexpr (std::is_base_of_v<CustomMouseInteractable, T>) {
      eventDispatcher.addMouseEventListener(result);
    }
    if constexpr (std::is_base_of_v<KeyboardInteractable, T>) {
      eventDispatcher.addKeyboardEventListener(result);
    }
    if constexpr (std::is_base_of_v<UIVisible, T>) {
      drawable.emplace_back(result);
    }
    return result;
  }

  void render(glm::mat4 projection = glm::mat4(1),
              glm::mat4 view = glm::mat4(1), glm::mat4 model = glm::mat4(1));

private:
  std::vector<std::weak_ptr<UIVisible>> drawable;
  EventDispatcher eventDispatcher;
  //FocusManager focusManager;

  std::shared_ptr<Window> window;

  GUIRenderer renderer;
};

} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_GUIMANAGER_H
