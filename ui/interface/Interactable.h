//
// Created by petr on 11/20/19.
//

#ifndef TERRAINGENERATION_INTERACTABLE_H
#define TERRAINGENERATION_INTERACTABLE_H

#include "UIObject.h"
namespace sdl2cpp::ui {
class Interactable : public virtual UIObject {
public:
  [[nodiscard]] bool areControlsEnabled() const;
  void enableControls();
  void disableControls();

protected:
  void onEnabledChanged(bool enabled) override;

private:
  bool controlsEnabled = true;
};
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_INTERACTABLE_H
