
// Repository: https://github.com/MrVallentin/glText
// License: https://github.com/MrVallentin/glText/blob/master/LICENSE.md
//
// Date Created: September 24, 2013
// Last Modified: October 30, 2018

// In one C or C++ file, define GLT_IMPLEMENTATION prior to inclusion to create the implementation.
//   #define GLT_IMPLEMENTATION
//   #include "gltext.h"

// Copyright (c) 2013-2018 Christian Vallentin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.

// Refrain from using any exposed macros, functions
// or structs prefixed with an underscore. As these
// are only intended for internal purposes. Which
// additionally means they can be removed, renamed
// or changed between minor updates without notice.

#ifndef GL_TEXT_H
#define GL_TEXT_H

#include <geGL/geGL.h>
#include <geGL/StaticCalls.h>

#ifdef __cplusplus
extern "C" {
#endif



#include <cstdlib> /* malloc(), calloc(), free() */
#include <cstring> /* memset(), memcpy(), strlen() */
#include <cstdint> /* uint8_t, uint16_t, uint32_t, uint64_t */

#if (defined(_DEBUG) || defined(DEBUG)) && !defined(GLT_DEBUG)
#	define GLT_DEBUG 1
#endif

#ifdef GLT_DEBUG
#	include <assert.h> /* assert() */
#	define _GLT_ASSERT(expression) assert(expression)
#else
#	define _GLT_ASSERT(expression)
#endif

#ifdef GLT_DEBUG_PRINT
#	include <stdio.h> /* printf */
#endif

#define _GLT_STRINGIFY(str) #str
#define _GLT_STRINGIFY_TOKEN(str) _GLT_STRINGIFY(str)

#define GLT_STRINGIFY_VERSION(major, minor, patch) _GLT_STRINGIFY(major) "." _GLT_STRINGIFY(minor) "." _GLT_STRINGIFY(patch)

#define GLT_NAME "glText"

#define GLT_VERSION_MAJOR 1
#define GLT_VERSION_MINOR 1
#define GLT_VERSION_PATCH 6

#define GLT_VERSION GLT_STRINGIFY_VERSION(GLT_VERSION_MAJOR, GLT_VERSION_MINOR, GLT_VERSION_PATCH)
#define GLT_NAME_VERSION GLT_NAME " " GLT_VERSION

#define GLT_NULL 0
#define GLT_NULL_HANDLE 0

#ifdef GLT_IMPORTS
#	define GLT_API extern
#else
#	ifndef GLT_STATIC
#		define GLT_STATIC
#	endif
#	define GLT_API static
#endif

#define GLT_LEFT 0
#define GLT_TOP 0

#define GLT_CENTER 1

#define GLT_RIGHT 2
#define GLT_BOTTOM 2

static GLboolean gltInitialized = GL_FALSE;

typedef struct GLTtext GLTtext;

GLT_API GLboolean gltInit(void);
GLT_API void gltTerminate(void);

GLT_API GLTtext* gltCreateText(void);
GLT_API void gltDeleteText(GLTtext *text);
#define gltDestroyText gltDeleteText

GLT_API GLboolean gltSetText(GLTtext *text, const char *string);
GLT_API const char* gltGetText(GLTtext *text);

GLT_API void gltViewport(GLsizei width, GLsizei height);

GLT_API void gltBeginDraw();
GLT_API void gltEndDraw();

GLT_API void gltDrawText(GLTtext *text, const GLfloat mvp[16]);

GLT_API void gltDrawText2D(GLTtext *text, GLfloat x, GLfloat y, GLfloat scale);
GLT_API void gltDrawText2DAligned(GLTtext *text, GLfloat x, GLfloat y, GLfloat scale, int horizontalAlignment, int verticalAlignment);

GLT_API void gltDrawText3D(GLTtext *text, GLfloat x, GLfloat y, GLfloat z, GLfloat scale, GLfloat view[16], GLfloat projection[16]);

GLT_API void gltColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
GLT_API void gltGetColor(GLfloat *r, GLfloat *g, GLfloat *b, GLfloat *a);

GLT_API GLfloat gltGetLineHeight(GLfloat scale);

GLT_API GLfloat gltGetTextWidth(const GLTtext *text, GLfloat scale);
GLT_API GLfloat gltGetTextHeight(const GLTtext *text, GLfloat scale);

GLT_API GLboolean gltIsCharacterSupported(const char c);
GLT_API GLint gltCountSupportedCharacters(const char *str);

GLT_API GLboolean gltIsCharacterDrawable(const char c);
GLT_API GLint gltCountDrawableCharacters(const char *str);

GLT_API GLint gltCountNewLines(const char *str);


#ifdef __cplusplus
}
#endif

#endif