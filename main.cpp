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

using namespace std;

string screen_state = "main_menu";

int main(int argc, char* argv[]) {
    SDL_Window* window = initSDL(Screen::WIDTH, Screen::HEIGHT, "Drum It!");
    SDL_Renderer* renderer = createRenderer(window);

    //init
    GameManager* game = nullptr;
    unordered_map<string, int> game_output;
    bool paused = false;
    SDL_Event event;
    bool running = true;
    Uint32 frame_start, frame_time;
    GameAudio::MenuAudioManager menu_music("sfx/music/menu_music.wav");
    // Menu
    Background background_menu(renderer, "assets/menu/background.png");
    Button play_button(renderer, "assets/menu/Play Button.png", []{
        screen_state = "levels_menu";
    });
    Button quit_button(renderer, "assets/menu/Quit Button.png", [&running, &menu_music]{
        running = false;
        menu_music.unpause();
    });

    // Levels
    Text text_choose_levels(renderer, "font/Aller_bd.ttf", 84, "Blue", "SELECT LEVEL");
    Button level_1(renderer, "assets/menu/lvl1.png", [&game, &renderer, &menu_music]{
        screen_state = "play";
        initialize_level(game, renderer, "map1");
        menu_music.stop();
    });
    Button level_2(renderer, "assets/menu/lvl2.png", [&game, &renderer, &menu_music]{
        screen_state = "play";
        initialize_level(game, renderer, "map2");
        menu_music.stop();
    });
    Button level_3(renderer, "assets/menu/lvl3.png", [&game, &renderer, &menu_music]{
        screen_state = "play";
        initialize_level(game, renderer, "map3");
        menu_music.stop();
    });
    Button level_4(renderer, "assets/menu/lvl4.png", [&game, &renderer, &menu_music]{
        screen_state = "play";
        initialize_level(game, renderer, "map4");
        menu_music.stop();
    });
    Button return_to_screen(renderer, "assets/end_screen/back.png", [&menu_music]{
        if (screen_state == "levels_menu") screen_state = "main_menu";
        if (screen_state == "end_game"){
            screen_state = "levels_menu";
            menu_music.play();
        }
    });

    // Gameplay
    Background* background_game = nullptr;
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
    Animation streak_flame(renderer, "assets/play/streak_animation_sheets.png", vector<SDL_Rect>{
        {0, 1, 384, 163}, {384, 1, 384, 163}, {768, 1, 384, 163}, {1152, 1, 384, 163},
        {0, 164, 384, 163}, {384, 164, 384, 163}, {768, 164, 384, 163}, {1152, 164, 384, 163},
        {0, 327, 384, 163}, {384, 327, 384, 163}, {768, 327, 384, 163}, {1152, 327, 384, 163},
        {0, 490, 384, 163}, {384, 490, 384, 163}, {768, 490, 384, 163}, {1152, 490, 384, 163},
        {0, 653, 384, 163}, {384, 653, 384, 163}, {768, 653, 384, 163}, {1152, 653, 384, 163},
        {0, 816, 384, 163}, {384, 816, 384, 163}, {768, 816, 384, 163}, {1152, 816, 384, 163},
        {0, 979, 384, 163}, {384, 979, 384, 163}, {768, 979, 384, 163}, {1152, 979, 384, 163},
        {0, 1142, 384, 163}, {384, 1142, 384, 163}, {768, 1142, 384, 163}, {1152, 1142, 384, 163}
    }, 25);
    //Pause
    Button pause_btn(renderer, "assets/play/pause.png", [&paused, &streak_flame, &game]{
        if (screen_state == "play" && game) {
            paused = !paused;
            if (paused) {
                game->game_time.pause();
                game->game_audio.pause();
                streak_flame.freeze = true;
            } else {
                game->game_time.unpause();
                game->game_audio.unpause();
                streak_flame.freeze = false;
            }
        }
    });
    Texture pause_board(renderer, "assets/play/pause_board.png");
    Button play_btn(renderer, "assets/play/play_btn.png", [&paused, &game,&streak_flame]{
        paused = false;
        game->game_time.unpause();
        game->game_audio.unpause();
        streak_flame.freeze = false;
    });
    Button restart_btn(renderer, "assets/play/restart_btn.png", [&game, &background_game, &paused, &renderer]{
        string bm_path = game->beatmap["directory"].get<string>();
        paused = false;
        if (game){
            delete game;
            game = nullptr;
        }
        if (background_game){
            delete background_game;
            background_game = nullptr;
        }
        initialize_level(game, renderer, bm_path);
    });
    Button menu_btn(renderer, "assets/play/menu_btn.png", [&game, &paused, &background_game, &menu_music]{
        screen_state = "levels_menu";
        menu_music.play();
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
    Texture pass(renderer, "assets/end_screen/pass.png");
    Texture fail(renderer, "assets/end_screen/fail.png");
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
    vector <Button*> play_buttons = {&pause_btn};
    vector <Button*> pause_screen_buttons = {&play_btn, &restart_btn, &menu_btn};
    vector <Button*> end_game_screen_buttons = {&return_to_screen};

    menu_music.play();
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
                            pause_btn.on_click();
                        } else if (screen_state == "levels_menu") {
                            screen_state = "main_menu";
                        } else if (screen_state == "end_game") {
                            screen_state = "levels_menu";
                            menu_music.play();
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
                else buttons = &play_buttons;
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
                                    pause_board, play_btn, restart_btn, menu_btn, streak_flame, pause_btn);
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
                            ok_notes, missed_notes, pass, fail, game_output);
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
    if (background_game){
        delete background_game;
        background_game = nullptr;
    }
    menu_music.stop();
    quitSDL(window, renderer);
    return 0;
}
