//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_GUIMANAGER_H
#define TERRAINGENERATION_GUIMANAGER_H

#include <ui/interface/ui_traits.h>
#include <utility>

#include "EventDispatcher.h"
#include "FocusManager.h"

namespace sdl2cpp::ui {
class GUIManager {
public:
  explicit GUIManager(std::shared_ptr<Window> window, const String &fontPath)
      : window(std::move(window)), eventDispatcher(window, focusManager),
        renderer(fontPath) {}
  GUIManager(const GUIManager &) = delete;
  GUIManager &operator=(const GUIManager &) = delete;

  template <typename T, typename... Args>
  [[nodiscard]] std::enable_if_t<
      is_ui_object<T> && std::is_constructible_v<T, GUIManager &, Args...>,
      std::shared_ptr<T>>
  createGUIObject(Args &&... args);

  void render(glm::mat4 projection = glm::mat4(1),
              glm::mat4 view = glm::mat4(1), glm::mat4 model = glm::mat4(1));

  FontManager &getFontManager();

private:
  std::vector<std::weak_ptr<UIVisible>> drawable;
  std::vector<std::weak_ptr<UIObject>> objects;
  EventDispatcher eventDispatcher;
  FocusManager focusManager{objects};

  std::shared_ptr<Window> window;

  GUIRenderer renderer;
};

template <typename T, typename... Args>
std::enable_if_t<is_ui_object<T> &&
                     std::is_constructible_v<T, GUIManager &, Args...>,
                 std::shared_ptr<T>>
GUIManager::createGUIObject(Args &&... args) {
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

#endif // TERRAINGENERATION_GUIMANAGER_H
