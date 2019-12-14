//
// Created by petr on 11/20/19.
//

#ifndef TERRAINGENERATION_UTILS_H
#define TERRAINGENERATION_UTILS_H

#include <SDL_rect.h>
#include <glm/vec3.hpp>

std::array<glm::vec3, 4> sdlRectToGLCoordinates(const SDL_Rect &rect,
                                                unsigned int screenWidth,
                                                unsigned int screenHeight);

#endif // TERRAINGENERATION_UTILS_H
