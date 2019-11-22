//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_EVENTDISPATCHER_H
#define TERRAINGENERATION_EVENTDISPATCHER_H

#include "ui/Fwd.h"
#include "ui/interface/KeyboardInteractable.h"
#include "ui/interface/MouseInteractable.h"
#include <SDL2CPP/Window.h>
#include <set>

namespace sdl2cpp::ui {

class EventDispatcher {
  friend class GUIManager;

public:
  explicit EventDispatcher(std::shared_ptr<Window> window);

private:
  void addMouseEventListener(
      std::weak_ptr<CustomMouseInteractable> mouseInteractable);
  void addKeyboardEventListener(
      std::weak_ptr<CustomKeyboardInteractable> keyboardInteractable);

  bool mouseEventHandler(const SDL_Event &event);

  bool keyboardEventHandler(const SDL_Event &event);

  CustomMouseInteractable *mouseDownIn = nullptr;
  CustomMouseInteractable *mouseIn = nullptr;

  std::optional<std::shared_ptr<CustomMouseInteractable>>
  findMouseInteractibleOnPosition(int x, int y);

  std::optional<std::shared_ptr<CustomKeyboardInteractable>>
  getFocusedKeyboardInteractible();

  std::shared_ptr<Window> window;
  std::vector<std::weak_ptr<CustomMouseInteractable>> mouseEventListeners;
  std::vector<std::weak_ptr<CustomKeyboardInteractable>> keyboardEventListeners;

  std::set<Window::EventType> keyboardEvents {SDL_KEYDOWN,
                                              SDL_KEYUP,
                                              SDL_TEXTEDITING,
                                              SDL_TEXTINPUT,
                                              SDL_KEYMAPCHANGED};
  std::set<Window::EventType> mouseEvents {SDL_MOUSEMOTION,
                                           SDL_MOUSEBUTTONDOWN,
                                           SDL_MOUSEBUTTONUP,
                                           SDL_MOUSEWHEEL};
};
}


#endif // TERRAINGENERATION_EVENTDISPATCHER_H
