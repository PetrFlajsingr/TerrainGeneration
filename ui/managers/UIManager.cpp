//
// Created by petr on 10/11/19.
//

#include "UIManager.h"
#include "ui/elements/Button.h"
#include <ui/elements/Label.h>

void sdl2cpp::ui::UIManager::render(glm::mat4 projection, glm::mat4 view, glm::mat4 model) {
  eventDispatcher.checkTimedEvents(now<std::chrono::milliseconds>());
  renderer.getProgram()->use();
  ge::gl::glEnable(GL_BLEND);
  ge::gl::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (const auto &element : drawable) {
    if (element.expired()) {
      continue;
    }
    auto ptr = element.lock();
    if (ptr->visibility.get() == Visibility::Visible) {
      ptr->draw(renderer);
    }
  }

  renderer.getTextRenderer().begin(projection, view, model);
  for (const auto &element : drawable) {
    if (element.expired()) {
      continue;
    }
    auto ptr = element.lock();
    if (ptr->visibility.get() == Visibility::Visible) {
      if (auto p = std::dynamic_pointer_cast<sdl2cpp::ui::Button>(ptr); p != nullptr) {
        renderer.getTextRenderer().render(p->text);
      } else if (auto p = std::dynamic_pointer_cast<sdl2cpp::ui::Label>(ptr); p != nullptr) {
        renderer.getTextRenderer().render(p->text);
      }
    }
  }
  renderer.getTextRenderer().end();
}
FontManager &sdl2cpp::ui::UIManager::getFontManager() { return renderer.getTextRenderer().getFontManager(); }
sdl2cpp::ui::FocusManager &sdl2cpp::ui::UIManager::getFocusManager() { return focusManager; }
TextRenderer &sdl2cpp::ui::UIManager::getTextRenderer() { return renderer.getTextRenderer(); }
