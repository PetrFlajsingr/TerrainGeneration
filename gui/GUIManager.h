//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_GUIMANAGER_H
#define TERRAINGENERATION_GUIMANAGER_H

#include <utility>

#include "EventDispatcher.h"
#include "FocusManager.h"

namespace sdl2cpp::gui {
class GUIManager {
public:
  explicit GUIManager(std::shared_ptr<Window> window)
      : window(std::move(window)), eventDispatcher(window) {}
  GUIManager(const GUIManager &) = delete;
  GUIManager &operator=(const GUIManager &) = delete;

  template <typename T, typename... Args>
  [[nodiscard]] std::enable_if_t<std::is_base_of_v<GUIObject, T> &&
                                     std::is_constructible_v<T, Args...>,
                                 std::shared_ptr<T>>
  createGUIObject(Args &&... args) {
    auto result = std::make_shared<T>(std::forward<Args>(args)...);
    if constexpr (std::is_base_of_v<MouseInteractable, T>) {
      eventDispatcher.addMouseEventListener(result);
    }
    if constexpr (std::is_base_of_v<KeyboardInteractable, T>) {
      eventDispatcher.addKeyboardEventListener(result);
    }
    return result;
  }

private:
  EventDispatcher eventDispatcher;
  //FocusManager focusManager;

  std::shared_ptr<Window> window;

  std::vector<std::weak_ptr<GUIObject>> guiObjects;
};

} // namespace sdl2cpp::gui

#endif // TERRAINGENERATION_GUIMANAGER_H
