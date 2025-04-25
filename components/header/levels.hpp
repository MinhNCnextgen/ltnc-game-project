// levels.hpp
#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "graphic.hpp"
#include "constants.hpp"
using namespace std;

void render_levels_menu(SDL_Renderer* renderer,
                        Background& bg,
                        Text& levels_text,
                        Button& return_button,
                        Button& level1,
                        Button& level2,
                        Button& level3,
                        Button& level4);
