//
// Created by petr on 10/11/19.
//

#include "GUIManager.h"
#include "ui/elements/Button.h"

void sdl2cpp::ui::GUIManager::render(glm::mat4 projection, glm::mat4 view,
                                     glm::mat4 model) {
  renderer.getProgram()->use();
  for (const auto &element : drawable) {
    if (element.expired()) {
      continue;
    }
    auto ptr = element.lock();
    ptr->draw(renderer);
  }

  renderer.getTextRenderer().begin(projection, view, model);
  for (const auto &element : drawable) {
    if (element.expired()) {
      continue;
    }
    auto ptr = element.lock();
    if (auto p = std::dynamic_pointer_cast<sdl2cpp::ui::Button>(ptr);
        p != nullptr) {
      renderer.getTextRenderer().render(p->text);
    }
  }
  renderer.getTextRenderer().end();
}
