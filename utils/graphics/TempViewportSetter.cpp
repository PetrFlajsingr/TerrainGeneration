//
// Created by petr on 12/17/19.
//

#include "TempViewportSetter.h"

TempViewportSetter::TempViewportSetter(const std::array<GLint, 4> &newViewport) {
  ge::gl::glGetIntegerv(GL_VIEWPORT, originalViewPort.data());
  const auto [x, y, width, height] = newViewport;
  ge::gl::glViewport(x, y, width, height);
}

TempViewportSetter::~TempViewportSetter() {
  const auto [x, y, width, height] = originalViewPort;
  ge::gl::glViewport(x, y, width, height);
}
