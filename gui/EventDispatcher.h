//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_EVENTDISPATCHER_H
#define TERRAINGENERATION_EVENTDISPATCHER_H

#include <SDL2CPP/Window.h>
#include <set>
#include "Fwd.h"

namespace sdl2cpp::gui {

class EventDispatcher {
  friend class GUIManager;
public:
  explicit EventDispatcher(std::shared_ptr<Window> window);

private:
  void addMouseEventListener(std::weak_ptr<MouseInteractable> mouseInteractable);
  void addKeyboardEventListener(std::weak_ptr<KeyboardInteractable> keyboardInteractable);

  bool mouseEventHandler(const SDL_Event &event);

  bool keyboardEventHandler(const SDL_Event &event);

  MouseInteractable *mouseDownIn = nullptr;
  MouseInteractable *mouseIn = nullptr;


  std::optional<std::shared_ptr<MouseInteractable>> findMouseInteractibleOnPosition(int x, int y);

  std::optional<std::shared_ptr<KeyboardInteractable>>
  getFocusedKeyboardInteractible();

  std::shared_ptr<Window> window;
  std::vector<std::weak_ptr<MouseInteractable>> mouseEventListeners;
  std::vector<std::weak_ptr<KeyboardInteractable>> keyboardEventListeners;

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
