// logic.hpp
#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>
#include "constants.hpp"
#include "graphic.hpp"
#include "json.hpp"
#include "audio.hpp"
using namespace std;
using json = nlohmann::json;
using AudioManager = GameAudio::InGameAudioManager;
class GameManager {
public:
    struct Note {
        float spawn_time;
        float hit_time;
        bool type;
        int index;
        Texture* note_mask;
        static unordered_map<int, Texture*> textures;
        int note_hp;
        float x_pos, y_pos, width, height;

        Note(float ht, float st, bool t, int indx, GameManager* gm);
        void spawn();
        void update_position(float new_x = -1);
        void delete_note(GameManager* gm);
        static Texture* get_note_texture(const char* path, int note_index, GameManager* gm);
    };

    struct TimeManager {
        Uint32 start_time, previous_time, current_time;
        Uint32 elapsed_time, last_frame_time;
        Uint32 pause_duration, pause_start_time;
        bool is_paused;

        TimeManager();
        void update();
        void start_timer();
        void pause();
        void unpause();
    };

    struct StatsManager {
        int health, point, multiplier, excellent_notes, great_notes, ok_notes, missed_notes, highest_streak;
        float speed, accuracy;
        std::string final;
        const char* last_note_score;
        StatsManager(int hp, int spd);
        void stats_update();
    };

    struct KeyManager {
        unordered_map<string, unordered_map<string, int>> key_map;
        GameManager* game;
        KeyManager(GameManager* gm);
        void reset(std::string key);
        void key_down(std::string key);
        void update();
    };

    SDL_Renderer* renderer;
    KeyManager key_press;
    AudioManager game_audio;
    int next_note_index;
    TimeManager game_time;
    StatsManager game_stats;
    std::vector<Note> note_list;
    std::vector<Note*> active_notes;
    bool in_game;          
    json beatmap;
    GameManager(SDL_Renderer* ren, json beatmap);
    ~GameManager();

    void game_update(bool is_paused);
    void update_point(float gap);
    std::unordered_map<std::string, int> get_final_stats();
};
void initialize_level(GameManager*& game, SDL_Renderer* renderer, std::string beatmap_path);
