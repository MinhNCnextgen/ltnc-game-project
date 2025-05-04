// game_display.cpp
#include "game_display.hpp"
#include <cmath>

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
                        Text& game_time_text, Texture& pause_board, Texture& menu_btn, Texture& restart_btn, Texture& play_btn, Animation& streak_flame, Button& pause_btn) {
    if (game){
        if (!bg){
            bg = new Background(renderer, ("beatmaps/" + string(game->beatmap["directory"]) + "background.png").c_str());
            streak_flame.freeze = false;
        }
        bg->render_background_blur();
        lane.render(0, GameUI::Lane::Y, Screen::WIDTH, GameUI::Lane::HEIGHT);
        hit_box.render(GameUI::Hitbox::X, GameUI::Hitbox::Y, GameUI::Hitbox::SIZE, GameUI::Hitbox::SIZE);
        drum.render(GameUI::Drum::X, GameUI::Drum::Y, GameUI::Drum::SIZE, GameUI::Drum::SIZE);

        game->game_update(paused);

        score.render(GameUI::Score::X, GameUI::Score::Y, GameUI::Score::WIDTH, GameUI::Score::HEIGHT,
            (to_string(game->game_stats.point)).c_str());
        streak.render(GameUI::Streak::X, GameUI::Streak::Y, GameUI::Streak::WIDTH, GameUI::Streak::HEIGHT,
                    ("x" + to_string(game->game_stats.multiplier - 1)).c_str());
        accuracy.render(GameUI::Accuracy::X, GameUI::Accuracy::Y, GameUI::Accuracy::WIDTH, GameUI::Accuracy::HEIGHT,
                    (to_string(int(round(game->game_stats.accuracy))) + "%").c_str());
        game_time_text.render(GameUI::Time::X, GameUI::Time::Y, GameUI::Time::WIDTH, GameUI::Time::HEIGHT,
                            (to_string(((game->game_audio.song_duration + 3) * 1000 - game->game_time.elapsed_time) / 1000) + "s").c_str());
        note_score.render(GameUI::NoteScore::X, GameUI::NoteScore::Y, GameUI::NoteScore::WIDTH, GameUI::NoteScore::HEIGHT,
                        game->game_stats.last_note_score,
                        string(game->game_stats.last_note_score) == "Excellent" ? "Blue" :
                        string(game->game_stats.last_note_score) == "Great" ? "Green" :
                        string(game->game_stats.last_note_score) == "Ok" ? "Yellow" : "Red");
        health.render(GameUI::Health::X, GameUI::Health::Y, GameUI::Health::WIDTH, GameUI::Health::HEIGHT,
                    ("Health: " + to_string(game->game_stats.health)).c_str());
        if (game->game_stats.multiplier-1 >= GameUI::StreakFlame::APPEAR) {
            streak_flame.render_next_frame(GameUI::StreakFlame::X, GameUI::StreakFlame::Y, GameUI::StreakFlame::WIDTH,GameUI::StreakFlame::HEIGHT);
        }
        if (!paused) {
            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_F]) {
                drum_left.render(GameUI::Drum::X, GameUI::Drum::Y, GameUI::Drum::SIZE, GameUI::Drum::SIZE);
            }
            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J]) {
                drum_right.render(GameUI::Drum::X, GameUI::Drum::Y, GameUI::Drum::SIZE, GameUI::Drum::SIZE);
            }
            pause_btn.render(Screen::WIDTH - 110, 10, 100, 100);
        } else {
            // Render pausing
            int pause_board_x = GameUI::PauseBoard::X;
            int pause_board_y = GameUI::PauseBoard::Y;
            pause_board.render(pause_board_x, pause_board_y, GameUI::PauseBoard::WIDTH, GameUI::PauseBoard::HEIGHT);

            int button_width = GameUI::PauseButton::WIDTH;
            int button_height = GameUI::PauseButton::HEIGHT;
            int button_spacing = GameUI::PauseButton::SPACING;

            int buttons_start_x = GameUI::PauseButton::START_X;
            int buttons_start_y = GameUI::PauseButton::START_Y;

            menu_btn.render(buttons_start_x, buttons_start_y, button_width, button_height);
            restart_btn.render(buttons_start_x, buttons_start_y + button_height + button_spacing, button_width, button_height);
            play_btn.render(buttons_start_x, buttons_start_y + 2 * (button_height + button_spacing), button_width, button_height);
        }
    

    }
}

void render_end_game(SDL_Renderer* renderer,
                     Background& bg,
                     Texture& scoreboard,
                     Button& return_btn,
                     Text& final_score,
                     Text& result,
                     Text& acc,
                     Text& streak_text,
                     Text& excellent,
                     Text& great,
                     Text& ok,
                     Text& missed, Texture& pass, Texture& fail,
                     unordered_map<string, int> game_output) {
    bg.render_background();
    scoreboard.render(EndGame::SCOREBOARD_X, EndGame::SCOREBOARD_Y, EndGame::SCOREBOARD_WIDTH, EndGame::SCOREBOARD_HEIGHT);
    return_btn.render(Menu::Button::RETURN_X, Menu::Button::RETURN_Y, Menu::Button::RETURN_SIZE, Menu::Button::RETURN_SIZE);

    int base_x = EndGame::SCOREBOARD_X + EndGame::TEXT_OFFSET_X;
    int base_y = EndGame::SCOREBOARD_Y + EndGame::TEXT_OFFSET_Y;
    
    final_score.render(base_x, base_y, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
                       ("Final Score: " + to_string(game_output["score"])).c_str());
    acc.render(base_x, base_y + EndGame::TEXT_SPACING, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
               ("Accuracy: " + to_string(game_output["accuracy"]) + "%").c_str());
    streak_text.render(base_x, base_y + EndGame::TEXT_SPACING * 2, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
                       ("Highest Streak: " + to_string(game_output["highest_streak"])).c_str());
    excellent.render(base_x, base_y + EndGame::TEXT_SPACING * 3, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
                     ("Excellent Notes: " + to_string(game_output["excellent_notes"])).c_str());
    great.render(base_x, base_y + EndGame::TEXT_SPACING * 4, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
                 ("Great Notes: " + to_string(game_output["great_notes"])).c_str());
    ok.render(base_x, base_y + EndGame::TEXT_SPACING * 5, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
              ("OK Notes: " + to_string(game_output["ok_notes"])).c_str());
    missed.render(base_x, base_y + EndGame::TEXT_SPACING * 6, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
                  ("Missed Notes: " + to_string(game_output["missed_notes"])).c_str());
    if (game_output["type"] == 1){
        pass.render((Screen::WIDTH + 100) / 2, (Screen::HEIGHT - 300) / 2, 300, 300);
    }else{
        fail.render((Screen::WIDTH + 100) / 2, (Screen::HEIGHT - 300) / 2, 300, 300);
    }
}
