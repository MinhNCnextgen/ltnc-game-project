#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "graphic.hpp"
#include "logic.hpp"
#include "constants.hpp"
#include "audio.hpp"
#include <unordered_map>

using namespace std;

void render_play_screen(SDL_Renderer* renderer, GameManager* game, bool paused,
                        Background*& bg,
                        Texture& lane,
                        Texture& hit_box,
                        Texture& drum,
                        Texture& drum_left,
                        Texture& drum_right,
                        Text& score,
                        Text& streak,
                        Text& note_score,
                        Text& health,
                        Text& accuracy,
                        Text& game_time,
                        Texture& pause_board, 
                        Texture& menu_btn, 
                        Texture& restart_btn, 
                        Texture& play_btn,
                        Animation& streak_flame, Button& pause_btn);

void render_end_game(SDL_Renderer* renderer,
                     Background& bg,
                     Texture& scoreboard,
                     Button& return_btn,
                     Text& final_score,
                     Text& result,
                     Text& acc,
                     Text& streak,
                     Text& excellent,
                     Text& great,
                     Text& ok,
                     Text& missed, Texture& pass, Texture& fail,
                     unordered_map<string, int> game_output);
