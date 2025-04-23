// game_display.cpp
#include "game_display.hpp"
#include <cmath>

void render_play_screen(SDL_Renderer* renderer, GameManager* game, bool paused,
                        Background& bg,
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
                        Text& game_time_text) {
    if (game){
        bg.render_background();
        lane.render(0, GameUI::Lane::Y, Screen::WIDTH, GameUI::Lane::HEIGHT);
        hit_box.render(GameUI::Hitbox::X, GameUI::Hitbox::Y, GameUI::Hitbox::SIZE, GameUI::Hitbox::SIZE);
        drum.render(GameUI::Drum::X, GameUI::Drum::Y, GameUI::Drum::SIZE, GameUI::Drum::SIZE);
    
        if (!paused) {
            game->game_update(paused);
            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_F]) {
                drum_left.render(GameUI::Drum::X, GameUI::Drum::Y, GameUI::Drum::SIZE, GameUI::Drum::SIZE);
            }
            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J]) {
                drum_right.render(GameUI::Drum::X, GameUI::Drum::Y, GameUI::Drum::SIZE, GameUI::Drum::SIZE);
            }
        } else {
            game->game_update(paused);
        }
    
        score.render(GameUI::Score::X, GameUI::Score::Y, GameUI::Score::WIDTH, GameUI::Score::HEIGHT,
                     ("Score: " + std::to_string(game->game_stats.point)).c_str());
        streak.render(GameUI::Streak::X, GameUI::Streak::Y, GameUI::Streak::WIDTH, GameUI::Streak::HEIGHT,
                      ("x" + std::to_string(game->game_stats.multiplier - 1)).c_str());
        accuracy.render(GameUI::Accuracy::X, GameUI::Accuracy::Y, GameUI::Accuracy::WIDTH, GameUI::Accuracy::HEIGHT,
                        ("Accuracy: " + std::to_string(int(round(game->game_stats.accuracy))) + "%").c_str());
        game_time_text.render(GameUI::Time::X, GameUI::Time::Y, GameUI::Time::WIDTH, GameUI::Time::HEIGHT,
                              ("Time Left: " + std::to_string(((game->game_audio.song_duration + 3) * 1000 - game->game_time.elapsed_time) / 1000) + "s").c_str());
        note_score.render(GameUI::NoteScore::X, GameUI::NoteScore::Y, GameUI::NoteScore::WIDTH, GameUI::NoteScore::HEIGHT,
                          game->game_stats.last_note_score,
                          std::string(game->game_stats.last_note_score) == "Excellent" ? "Blue" :
                          std::string(game->game_stats.last_note_score) == "Great" ? "Green" :
                          std::string(game->game_stats.last_note_score) == "Ok" ? "Yellow" : "Red");
        health.render(GameUI::Health::X, GameUI::Health::Y, GameUI::Health::WIDTH, GameUI::Health::HEIGHT,
                      ("Health: " + std::to_string(game->game_stats.health)).c_str());
    }
}

void render_end_game(SDL_Renderer* renderer,
                     Background& bg,
                     Texture& scoreboard,
                     MenuButton& return_btn,
                     Text& final_score,
                     Text& result,
                     Text& acc,
                     Text& streak_text,
                     Text& excellent,
                     Text& great,
                     Text& ok,
                     Text& missed,
                     std::unordered_map<std::string, int> game_output) {
    bg.render_background();
    scoreboard.render(EndGame::SCOREBOARD_X, EndGame::SCOREBOARD_Y, EndGame::SCOREBOARD_WIDTH, EndGame::SCOREBOARD_HEIGHT);
    return_btn.render(Menu::Button::RETURN_X, Menu::Button::RETURN_Y, Menu::Button::RETURN_SIZE, Menu::Button::RETURN_SIZE);

    int base_x = EndGame::SCOREBOARD_X + EndGame::TEXT_OFFSET_X;
    int base_y = EndGame::SCOREBOARD_Y + EndGame::TEXT_OFFSET_Y;
    
    final_score.render(base_x, base_y, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
                       ("Final Score: " + std::to_string(game_output["score"])).c_str());
    result.render(base_x, base_y + EndGame::TEXT_SPACING, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
                  ("Result: " + std::string(game_output["type"] == 1 ? "Pass" : "Fail")).c_str());
    acc.render(base_x, base_y + EndGame::TEXT_SPACING * 2, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
               ("Accuracy: " + std::to_string(game_output["accuracy"]) + "%").c_str());
    streak_text.render(base_x, base_y + EndGame::TEXT_SPACING * 3, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
                       ("Highest Streak: " + std::to_string(game_output["highest_streak"])).c_str());
    excellent.render(base_x, base_y + EndGame::TEXT_SPACING * 4, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
                     ("Excellent Notes: " + std::to_string(game_output["excellent_notes"])).c_str());
    great.render(base_x, base_y + EndGame::TEXT_SPACING * 5, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
                 ("Great Notes: " + std::to_string(game_output["great_notes"])).c_str());
    ok.render(base_x, base_y + EndGame::TEXT_SPACING * 6, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
              ("OK Notes: " + std::to_string(game_output["ok_notes"])).c_str());
    missed.render(base_x, base_y + EndGame::TEXT_SPACING * 7, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT,
                  ("Missed Notes: " + std::to_string(game_output["missed_notes"])).c_str());
}
