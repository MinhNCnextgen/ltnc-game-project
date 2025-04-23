// menu.hpp
#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "graphic.hpp"
#include "constants.hpp"
using namespace std;

void render_main_menu(SDL_Renderer* renderer, Background& bg, MenuButton& play_btn, MenuButton& quit_btn);
