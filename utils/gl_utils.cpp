//
// Created by petr on 10/30/19.
//

#include "gl_utils.h"
bool checkProgramLinkStatus(GLuint program) {
  GLint isLinked;
  ge::gl::glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
  if (isLinked == GL_FALSE) {
    int length;
    std::string log;
    ge::gl::glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    log.resize(static_cast<unsigned long>(length));
    ge::gl::glGetProgramInfoLog(program, length, &length, &log[0]);

    ge::gl::glDeleteProgram(program);
    std::cerr << "cannot Link: " << log << std::endl;
    return false;
  }
  return true;
}
