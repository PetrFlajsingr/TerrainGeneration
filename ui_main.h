//
// Created by petr on 12/5/19.
//

#ifndef TERRAINGENERATION_UI_MAIN_H
#define TERRAINGENERATION_UI_MAIN_H

#include "rendering/Data.h"
#include "rendering/shadow_maps/CascadedShadowMap.h"
#include "rendering/textures/DrawTexture.h"
#include "ui/elements.h"
#include "ui/managers.h"
#include "various/loc_assert.h"
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <String.h>
#include <config/JsonConfig.h>
#include <graphics/gl_utils.h>
#include <rendering/models/ModelRenderer.h>

using Conf = JsonConfig<true>;

void ui_main(int argc, char *argv[]);

#endif // TERRAINGENERATION_UI_MAIN_H
