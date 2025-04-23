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
                        MenuButton& return_button,
                        MenuButton& level1,
                        MenuButton& level2,
                        MenuButton& level3,
                        MenuButton& level4);
