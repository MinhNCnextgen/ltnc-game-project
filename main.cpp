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

    //init
    GameManager* game = nullptr;
    std::unordered_map<std::string, int> game_output;
    bool paused = false;
    SDL_Event event;
    bool running = true;
    Uint32 frame_start, frame_time;
    // Menu
    Background background_menu(renderer, "assets/menu/background.png");
    Button play_button(renderer, "assets/menu/Play Button.png", []{
        screen_state = "levels_menu";
    });
    Button quit_button(renderer, "assets/menu/Quit Button.png", [&running]{
        running = false;
    });

    // Levels
    Text text_choose_levels(renderer, "font/Aller_bd.ttf", 84, "Blue", "SELECT LEVEL");
    Button level_1(renderer, "assets/menu/lvl1.png", [&game, &renderer]{
        screen_state = "play";
        initialize_level(game, renderer, "map1");
    });
    Button level_2(renderer, "assets/menu/lvl2.png", [&game, &renderer]{
        screen_state = "play";
        initialize_level(game, renderer, "map2");
    });
    Button level_3(renderer, "assets/menu/lvl3.png", [&game, &renderer]{
        screen_state = "play";
        initialize_level(game, renderer, "map3");
    });
    Button level_4(renderer, "assets/menu/lvl4.png", [&game, &renderer]{
        screen_state = "play";
        initialize_level(game, renderer, "map4");
    });
    Button return_to_screen(renderer, "assets/end_screen/back.png", []{
        if (screen_state == "levels_menu") screen_state = "main_menu";
        if (screen_state == "end_game") screen_state = "levels_menu";
    });

    // Gameplay
    Background* background_game;
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
    //Pause
    Texture pause_board(renderer, "assets/play/pause_board.png");
    Button play_btn(renderer, "assets/play/play_btn.png", [&paused, &game]{
        paused = false;
        game->game_time.unpause();
        game->game_audio.unpause();
    });
    Button restart_btn(renderer, "assets/play/restart_btn.png", [&game]{
        // game->restart();
    });
    Button menu_btn(renderer, "assets/play/menu_btn.png", [&game, &paused, &background_game]{
        screen_state = "levels_menu";
        paused = false;
        if (game){
            delete game;
            game = nullptr;
        }
        if (background_game){
            delete background_game;
            background_game = nullptr;
        }
    });

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

    //Button list
    vector <Button*> menu_screen_buttons = {&play_button, &quit_button};
    vector <Button*> choose_level_screen_buttons = {&level_1, &level_2, &level_3, &level_4, &return_to_screen};
    vector <Button*> pause_screen_buttons = {&play_btn, &restart_btn, &menu_btn};
    vector <Button*> end_game_screen_buttons = {&return_to_screen};
    while (running) {
        frame_start = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
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
            vector<Button*>* buttons = nullptr;

            if (screen_state == "main_menu") {
                buttons = &menu_screen_buttons;
            } else if (screen_state == "levels_menu") {
                buttons = &choose_level_screen_buttons;
            } else if (screen_state == "end_game") {
                buttons = &end_game_screen_buttons;
            } else if (screen_state == "play"){
                if (paused) buttons = &pause_screen_buttons;
                else buttons = nullptr;
            }

            if (buttons) {
                bool hovered = false;
                for (auto& button : *buttons) {
                    if (!hovered && button->is_hovering()){
                        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
                        hovered = true;
                    }
                    if (!hovered) SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                    
                    button->is_click(&event);
                }
            }else{
                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
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
                                   drum_left, drum_right, score, streak, note_score, health, accuracy, game_time,
                                    pause_board, play_btn, restart_btn, menu_btn);
                SDL_RenderPresent(renderer);
            } else {
                if (game != nullptr) { 
                    game_output = game->get_final_stats();
                    delete game;
                    game = nullptr;
                    screen_state = "end_game";
                    paused = false;
                }
                if (background_game){
                    delete background_game;
                    background_game = nullptr;
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
