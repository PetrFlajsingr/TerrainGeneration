//
// Created by petr on 10/11/19.
//

#include "GUIManager.h"

void sdl2cpp::ui::GUIManager::render() {
  renderer.getProgram()->use();
  for (const auto &element : drawable) {
    if (element.expired()) {
      continue;
    }
    auto ptr = element.lock();
    ptr->draw(renderer);
  }
}
