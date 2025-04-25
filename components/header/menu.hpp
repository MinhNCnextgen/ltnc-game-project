// menu.hpp
#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "graphic.hpp"
#include "constants.hpp"
using namespace std;

void render_main_menu(SDL_Renderer* renderer, Background& bg, Button& play_btn, Button& quit_btn);
