// main.cpp
#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include "constants.hpp"
#include "graphic.hpp"
#include "logic.hpp"
#include "menu.hpp"
#include "levels.hpp"
#include "game_display.hpp"
#include "audio.hpp"

std::string screen_state = "main_menu";

int main(int argc, char* argv[]) {
    SDL_Window* window = initSDL(Screen::WIDTH, Screen::HEIGHT, "Drum IT!");
    SDL_Renderer* renderer = createRenderer(window);

    // Menu
    Background background_menu(renderer, "assets/menu/background.png");
    MenuButton play_button(renderer, "assets/menu/Play Button.png");
    MenuButton quit_button(renderer, "assets/menu/Quit Button.png");

    // Levels
    Text text_choose_levels(renderer, "font/Aller_bd.ttf", 84, "Blue", "SELECT LEVEL");
    MenuButton level_1(renderer, "assets/menu/lvl1.png");
    MenuButton level_2(renderer, "assets/menu/lvl2.png");
    MenuButton level_3(renderer, "assets/menu/lvl3.png");
    MenuButton level_4(renderer, "assets/menu/lvl4.png");
    MenuButton return_to_screen(renderer, "assets/end_screen/back.png");

    // Gameplay
    Background background_game(renderer, "assets/play/game_bg.png");
    Texture drum(renderer, "assets/play/drum.png");
    Texture drum_left(renderer, "assets/play/drum_left.png");
    Texture drum_right(renderer, "assets/play/drum_right.png");
    Texture hit_box(renderer, "assets/play/hitbox.png");
    Texture lane(renderer, "assets/play/lane.png");

    Text score(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "White", "");
    Text streak(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "White", "");
    Text note_score(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "White", "");
    Text health(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "White", "");
    Text accuracy(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "White", "");
    Text game_time(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "White", "");

    // End game
    Texture score_board(renderer, "assets/end_screen/scoreboard.png");
    Text final_score(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text game_result(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text final_acc(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text highest_streak(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text great_notes(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text excellent_notes(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text ok_notes(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text missed_notes(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");

    GameManager* game = nullptr;
    std::unordered_map<std::string, int> game_output;
    bool paused = false;

    SDL_Event event;
    bool running = true;
    Uint32 frame_start, frame_time;

    while (running) {
        frame_start = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (screen_state == "main_menu") {
                        if (play_button.is_hovering()) screen_state = "levels_menu";
                        else if (quit_button.is_hovering()) running = false;
                    } else if (screen_state == "levels_menu") {
                        if (level_1.is_hovering()) {
                            screen_state = "play";
                            initialize_level(game, renderer, "map1");
                        } else if (level_2.is_hovering()) {
                            screen_state = "play";
                            initialize_level(game, renderer, "map2");
                        } else if (level_3.is_hovering()) {
                            screen_state = "play";
                            initialize_level(game, renderer, "map3");
                        } else if (level_4.is_hovering()) {
                            screen_state = "play";
                            initialize_level(game, renderer, "map4");
                        } else if (return_to_screen.is_hovering()) {
                            screen_state = "main_menu";
                        }
                    } else if (screen_state == "end_game") {
                        if (return_to_screen.is_hovering()) screen_state = "levels_menu";
                    }
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        if (screen_state == "play" && game) {
                            paused = !paused;
                            if (paused) {
                                game->game_time.pause();
                                game->game_audio.pause();
                            } else {
                                game->game_time.unpause();
                                game->game_audio.unpause();
                            }
                        } else if (screen_state == "levels_menu") {
                            screen_state = "main_menu";
                        } else if (screen_state == "end_game") {
                            screen_state = "levels_menu";
                        }
                    }
                    break;
            }
        }

        if (screen_state == "main_menu") {
            render_main_menu(renderer, background_menu, play_button, quit_button); 
            SDL_RenderPresent(renderer);

        } else if (screen_state == "levels_menu") {
            render_levels_menu(renderer, background_menu, text_choose_levels, return_to_screen,
                               level_1, level_2, level_3, level_4);
            SDL_RenderPresent(renderer);
        } else if (screen_state == "play") {
            if (game && game->in_game) { 
                render_play_screen(renderer, game, paused, background_game, lane, hit_box, drum,
                                   drum_left, drum_right, score, streak, note_score, health, accuracy, game_time);
                SDL_RenderPresent(renderer);
            } else {
                if (game != nullptr) { 
                    game_output = game->get_final_stats();
                    delete game;
                    game = nullptr;
                    screen_state = "end_game";
                }
            }

        } else if (screen_state == "end_game") {
            render_end_game(renderer, background_menu, score_board, return_to_screen, final_score,
                            game_result, final_acc, highest_streak, excellent_notes, great_notes,
                            ok_notes, missed_notes, game_output);
            SDL_RenderPresent(renderer);
        }

        frame_time = SDL_GetTicks() - frame_start;
        if (GameTiming::FRAME_DELAY > frame_time) {
            SDL_Delay(GameTiming::FRAME_DELAY - frame_time);
        }
        SDL_RenderClear(renderer);
    }

    if (game) {
        delete game;
        game = nullptr;
    }
    quitSDL(window, renderer);
    return 0;
}
