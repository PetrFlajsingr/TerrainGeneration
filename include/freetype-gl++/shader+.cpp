/* ============================================================================
 * Freetype GL++ - A C++ FreeType-GL wrapper
 * Platform:       Any
 * WWW:            https://github.com/henriquegemignani/freetype-glpp
 * ----------------------------------------------------------------------------
 * Copyright 2013 Henrique Gemignani. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY HENRIQUE GEMIGNANI ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL HENRIQUE GEMIGNANI OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Henrique Gemignani.
 * ============================================================================
 *
 *  This library is merely a wrapper of the Freetype GL by Nicolas P. Rougier.
 *  WWW: http://code.google.com/p/freetype-gl/
 *
 * ============================================================================
 */
#include <freetype-gl++/shader+.hpp>

#include "shader.h"

namespace freetypeglxx {
namespace shader {

char *Read(const char *filename) { return shader_read(filename); }

GLuint Compile(const char *source, const GLenum type) {
  return shader_compile(source, type);
}

GLuint Load(const char *vert_filename, const char *frag_filename) {
  return shader_load(vert_filename, frag_filename);
}

// Function doesn't exists in freetype-gl
/* GLuint Get( GLuint self,
            const char * name ) {
    return shader_get(self, name);
}*/

/** @} */

} // namespace shader
} // namespace freetypeglxx
