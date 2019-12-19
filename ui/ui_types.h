//
// Created by petr on 10/11/19.
//

#ifndef TERRAINGENERATION_UI_TYPES_H
#define TERRAINGENERATION_UI_TYPES_H

#include "Fwd.h"
#include <SDL_keycode.h>
#include <SDL_rect.h>
#include <functional>

namespace sdl2cpp::ui {
enum class Visibility : bool { Visible, Invisible };

enum class Focus : bool { Focused, NotFocused };

enum class MouseButton { Left = 0, Right = 1, Middle = 2 };
constexpr unsigned int MouseButtonCount = 3;
enum class MouseButtonState { Pressed, Released };

enum class ScrollDirection { Left, Right, Up, Down };

namespace Event {
using DrawFnc = std::function<void(EventInfo)>;
using VisibilityChangedFnc = std::function<void(EventInfo, Visibility)>;
using FocusChangedFnc = std::function<void(EventInfo, Focus)>;
using EnabledChangedFnc = std::function<void(EventInfo, bool)>;

using MouseDownFnc = std::function<void(EventInfo, MouseButton, SDL_Point)>;
using MouseUpFnc = std::function<void(EventInfo, MouseButton, SDL_Point)>;
using MouseClickedFnc = std::function<void(EventInfo, MouseButton, SDL_Point)>;
using MouseDblClickedFnc = std::function<void(EventInfo, MouseButton, SDL_Point)>;
using MouseOverFnc = std::function<void(EventInfo)>;
using MouseOutFnc = std::function<void(EventInfo)>;
using MouseMoveFnc = std::function<void(EventInfo, SDL_Point, SDL_Point)>;
using MouseWheelFnc = std::function<void(EventInfo, ScrollDirection, int)>;

using KeyUpFnc = std::function<void(EventInfo, SDL_Keycode)>;
using KeyDownFnc = std::function<void(EventInfo, SDL_Keycode)>;
using KeyPressedFnc = std::function<void(EventInfo, SDL_Keycode)>;

enum class Type {
  Draw,
  VisibilityChanged,
  FocusChanged,
  EnabledChanged,
  MouseDown,
  MouseUp,
  MouseClicked,
  MouseDblClicked,
  MouseOver,
  MouseOut,
  MouseMove,
  MouseWheel,
  KeyDown,
  KeyUp,
  KeyPressed
};

template <Type type, Type ...types>
constexpr bool is_one_of_v = ((type == types) || ...);
} // namespace Event

struct EventInfo {
  UIObject &sender;
  Event::Type eventType;
};
} // namespace sdl2cpp::ui

#endif // TERRAINGENERATION_UI_TYPES_H
