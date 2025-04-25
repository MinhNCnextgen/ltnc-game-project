// logic.cpp
#include "logic.hpp"
#include <iostream>

using namespace std;

// Define static member
std::unordered_map<int, Texture*> GameManager::Note::textures;
 
// --- GameManager Methods ---

GameManager::GameManager(SDL_Renderer* ren, json bm)
    : renderer(ren),
      beatmap(bm),
      game_audio(("beatmaps/" + std::string(bm["directory"]) + "/song.mp3").c_str(), "sfx/sound/drum-hitclap.wav", "sfx/sound/drum-hitfinish.wav"),
      next_note_index(0),
      game_stats(bm["settings"]["default_health"], bm["settings"]["default_speed"]),
      in_game(true),
      key_press(this)
{
    for (size_t i = 0; i < bm["note_list"].size(); ++i) {
        float hit_time = bm["note_list"][i][1].get<float>() * 1000.0f;
        float distance = Screen::WIDTH + 100.0f;
        float time_needed = (distance / game_stats.speed) * 1000.0f;
        float spawn_time = hit_time - time_needed;
        bool note_type = (bm["note_list"][i][0] != 0);
        note_list.emplace_back(hit_time, spawn_time, note_type, i, this);
    }
}

GameManager::~GameManager() {
    for (auto& pair : GameManager::Note::textures) { 
        if (pair.second) {
            delete pair.second;
        }
    }
    GameManager::Note::textures.clear();
    game_audio.stop();
    note_list.clear();
    active_notes.clear();
}

void GameManager::game_update(bool is_paused) {
    if (!is_paused && in_game) {
        game_time.update();

        if (!note_list.empty() && next_note_index < static_cast<int>(note_list.size())) {
            Note& note = note_list[next_note_index];
            if (game_time.elapsed_time >= note.spawn_time) {
                note.spawn();
                active_notes.push_back(&note);
                next_note_index++;
            }
        }

        float new_x;
        for (auto it = active_notes.begin(); it != active_notes.end();) {
            Note* current_note = *it;
            if (!current_note) {
                it = active_notes.erase(it); // Remove null pointers
                continue;
            }

            new_x = current_note->x_pos - (game_stats.speed * game_time.last_frame_time / 1000.0f);
            if (new_x < 125) {
                current_note->delete_note(this);
                it = active_notes.erase(it);
                game_stats.multiplier = 1;
                game_stats.health--;
                game_stats.last_note_score = "Missed";
                game_stats.stats_update();
            } else {
                current_note->update_position(new_x);
                ++it;
            }
        }

        key_press.update();
        if (!active_notes.empty()) {
            Note* front = nullptr;
            float gap = 0.0f;
            for (auto note : active_notes) {
                gap = note->hit_time - game_time.elapsed_time;
                if (gap <= Game::Note::MISS_UPPER && gap >= Game::Note::MISS_LOWER && note->x_pos <= Game::Note::HIT_X) {
                    front = note;
                    break;
                }
            }

            if (front) {
                bool hit = (key_press.key_map["key_blue"]["is_holding"] && !front->type) || (key_press.key_map["key_orange"]["is_holding"] && front->type);
                bool wrong_hit = (key_press.key_map["key_blue"]["is_holding"] && front->type) || (key_press.key_map["key_orange"]["is_holding"] && !front->type);
                if (hit) {
                    update_point(gap);
                    front->delete_note(this);
                    active_notes.erase(find(active_notes.begin(), active_notes.end(), front));
                    game_stats.stats_update();
                } else if (wrong_hit) {
                    front->note_hp--;
                    if (front->note_hp <= 1) {
                        active_notes.erase(find(active_notes.begin(), active_notes.end(), front));
                        game_stats.multiplier = 1;
                        game_stats.health--;
                        game_stats.last_note_score = "Missed";
                        game_stats.stats_update();
                    }
                }
            }
        }

        if (game_stats.health <= 0) {
            game_stats.final = "lose";
            in_game = false;
        }
        if (game_time.elapsed_time > game_audio.song_duration * 1000.0f + 3000.0f) {
            game_stats.final = "win";
            in_game = false;
        }
    } else {
        for (auto note : active_notes) note->update_position();
    }
}

void GameManager::update_point(float gap) {
    if (gap <= Game::Note::MISS_UPPER && gap >= Game::Note::OK_UPPER) {
        game_stats.multiplier = 1;
        game_stats.health--;
        game_stats.last_note_score = "Missed";
    } else if (gap < Game::Note::OK_UPPER && gap >= Game::Note::GREAT_UPPER) {
        game_stats.point += Game::Note::OK_SCORE * game_stats.multiplier++;
        game_stats.last_note_score = "Ok";
    } else if (gap < Game::Note::GREAT_UPPER && gap >= Game::Note::EXCELLENT_UPPER) {
        game_stats.point += Game::Note::GREAT_SCORE * game_stats.multiplier++;
        game_stats.last_note_score = "Great";
    } else if (gap < Game::Note::EXCELLENT_UPPER && gap >= Game::Note::EXCELLENT_LOWER) {
        game_stats.point += Game::Note::EXCELLENT_SCORE * game_stats.multiplier++;
        game_stats.last_note_score = "Excellent";
    } else if (gap < Game::Note::EXCELLENT_LOWER && gap >= Game::Note::GREAT_LOWER) {
        game_stats.point += Game::Note::GREAT_SCORE * game_stats.multiplier++;
        game_stats.last_note_score = "Great";
    } else if (gap < Game::Note::GREAT_LOWER && gap >= Game::Note::OK_LOWER) {
    } else {
        game_stats.multiplier = 1;
        game_stats.health--;
        game_stats.last_note_score = "Missed";
    }
}

unordered_map<string, int> GameManager::get_final_stats() {
    return {
        {"type", game_stats.final == "win" ? 1 : 0},
        {"score", game_stats.point},
        {"accuracy", static_cast<int>(game_stats.accuracy)},
        {"highest_streak", game_stats.highest_streak},
        {"excellent_notes", game_stats.excellent_notes},
        {"great_notes", game_stats.great_notes},
        {"ok_notes", game_stats.ok_notes},
        {"missed_notes", game_stats.missed_notes}
    };
}


// --- Note Methods ---
GameManager::Note::Note(float ht, float st, bool t, int indx, GameManager* gm)
    : spawn_time(st), hit_time(ht), type(t), index(indx), note_hp(2) {
    note_mask = get_note_texture(t ? "assets/play/note_orange.png" : "assets/play/note_blue.png", (*gm).note_list.size(), gm);
    width = Game::Note::WIDTH;
    height = Game::Note::HEIGHT;
    x_pos = Game::Note::SPAWN_X;
    if (spawn_time < 0) {
        x_pos -= (*gm).game_stats.speed * (-spawn_time / 1000.0f);
    }
    y_pos = Game::Note::Y;
}

void GameManager::Note::spawn() {
    note_mask->render(x_pos, y_pos, width, height);
}

void GameManager::Note::update_position(float new_x) {
    if (new_x != -1) x_pos = new_x;
    note_mask->render(x_pos, y_pos, width, height);
}

void GameManager::Note::delete_note(GameManager* gm) {
    if (note_mask) {
        note_mask->destroy();
        note_mask = nullptr;
    }
    if (textures.count(index)) {
        delete textures[index];
        textures.erase(index);
    }
}

Texture* GameManager::Note::get_note_texture(const char* path, int note_index, GameManager* gm) {
    if (textures.find(note_index) == textures.end()) {
        textures[note_index] = new Texture(gm->renderer, path);
    }
    return textures[note_index];
}

// --- TimeManager Methods ---
GameManager::TimeManager::TimeManager()
    : start_time(0), previous_time(0), current_time(0), elapsed_time(0), last_frame_time(0), pause_duration(0), pause_start_time(0), is_paused(false) {}

void GameManager::TimeManager::update() {
    if (!is_paused) {
        previous_time = current_time;
        current_time = SDL_GetTicks() - pause_duration;
        elapsed_time = current_time - start_time;
        last_frame_time = current_time - previous_time;
        if (last_frame_time > static_cast<int>(GameTiming::FPS)) {
            last_frame_time = GameTiming::FPS;
        }
    }
}

void GameManager::TimeManager::start_timer() {
    Uint32 current_ticks = SDL_GetTicks();
    start_time = current_ticks;
    previous_time = current_ticks;
    current_time = current_ticks;
    elapsed_time = 0;
    last_frame_time = 0;
    pause_duration = 0;
    is_paused = false;
}

void GameManager::TimeManager::pause() {
    if (!is_paused) {
        pause_start_time = SDL_GetTicks();
        is_paused = true;
    }
}

void GameManager::TimeManager::unpause() {
    if (is_paused) {
        pause_duration += SDL_GetTicks() - pause_start_time;
        is_paused = false;
    }
}

// --- StatsManager Methods ---
GameManager::StatsManager::StatsManager(int hp, int spd)
    : health(hp), point(0), multiplier(1), excellent_notes(0), great_notes(0),
      ok_notes(0), missed_notes(0), highest_streak(0), speed(spd), accuracy(0.0f),
      last_note_score("") {
}

void GameManager::StatsManager::stats_update() {
    highest_streak = max(highest_streak, multiplier - 1);
    if (last_note_score == std::string("Excellent")) {
        excellent_notes++;
    } else if (last_note_score == std::string("Great")) {
        great_notes++;
    } else if (last_note_score == std::string("Ok")) {
        ok_notes++;
    } else if (last_note_score == std::string("Missed")) {
        missed_notes++;
    }
    float total_notes = excellent_notes + great_notes + ok_notes + missed_notes;
    if (total_notes > 0) {
        accuracy = ((excellent_notes * 300.0f + great_notes * 100.0f + ok_notes * 50.0f) / (total_notes * 300.0f)) * 100.0f;
    } else {
        accuracy = 0.0f;
    }
}

// --- KeyManager Methods ---
GameManager::KeyManager::KeyManager(GameManager * gm) : game(gm) {
    key_map = {
        {"key_blue", {{"time_press", 0}, {"time_held", 0}, {"is_holding", 0}}},
        {"key_orange", {{"time_press", 0}, {"time_held", 0}, {"is_holding", 0}}},
    };
}

void GameManager::KeyManager::reset(string key) {
    key_map[key]["time_press"] = 0;
    key_map[key]["time_held"] = 0;
    key_map[key]["is_holding"] = 0;
}

void GameManager::KeyManager::key_down(string key) {
    unordered_map<string, int>& key_val = key_map.at(key);
    if (key_val.at("is_holding") == 0) {
        key_val.at("is_holding") = 1;
        if (key_val.at("time_press") == 0) key_val.at("time_press") = SDL_GetTicks();
    }
}

void GameManager::KeyManager::update() {
    for (auto& pair : key_map) {
        if ((pair.first == "key_blue" ? SDL_GetKeyboardState(NULL)[SDL_SCANCODE_F] : SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J])) {
            key_down(pair.first);
            unordered_map<string, int>& key_val = pair.second;
            if (key_val.at("is_holding") == 1) {
                key_val.at("time_held") = SDL_GetTicks() - key_val.at("time_press");
                if (key_val.at("time_held") > 0) {
                    key_val.at("is_holding") = 0;
                }else{
                    if (pair.first == "key_blue") game->game_audio.play_hitsound(1);
                    if (pair.first == "key_orange") game->game_audio.play_hitsound(0);
                }
            }
        } else {
            reset(pair.first);
        }
    }
}


void initialize_level(GameManager*& game, SDL_Renderer* renderer, string beatmap_path) {
    if (game) {
        delete game; 
        game = nullptr;
    }

    string file_path = "beatmaps/" + beatmap_path + "/data.json";
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file at " << file_path << endl;
        return;
    }

    json beatmap;
    file >> beatmap;
    file.close();

    game = new GameManager(renderer, beatmap);
    game->game_audio.play_song();
    game->game_time.start_timer();
    game->in_game = true;
}
