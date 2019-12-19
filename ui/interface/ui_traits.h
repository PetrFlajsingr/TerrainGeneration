//
// Created by petr on 11/23/19.
//

#ifndef TERRAINGENERATION_UI_TRAITS_H
#define TERRAINGENERATION_UI_TRAITS_H

#include "ui/Fwd.h"
#include <type_traits>

namespace sdl2cpp::ui {

template <typename T> concept C_UIObject = std::is_base_of_v<UIObject, T>;
template <typename T> concept C_UIVisible = std::is_base_of_v<UIVisible, T>;

template <typename T> constexpr bool is_ui_object = std::is_base_of_v<UIObject, T>;
template <typename T> constexpr bool is_ui_drawable = std::is_base_of_v<UIVisible, T>;
template <typename T> constexpr bool is_keyboard_interactable = std::is_base_of_v<CustomKeyboardInteractable, T>;
template <typename T> constexpr bool is_mouse_interactable = std::is_base_of_v<CustomMouseInteractable, T>;

} // namespace sdl2cpp::ui
#endif // TERRAINGENERATION_UI_TRAITS_H
