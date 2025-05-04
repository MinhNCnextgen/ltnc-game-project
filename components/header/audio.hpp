#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include <iostream>

class GameAudio{
    public:
    struct InGameAudioManager {
        Mix_Music* song;
        Mix_Chunk* hit_sound_left;
        Mix_Chunk* hit_sound_right;
        Mix_Chunk* combobreak_sound;
        Uint32 song_duration;
        int channel;
        InGameAudioManager(const char* song_path, const char* hitsound_path_left, const char* hitsound_path_right, const char* combobreak_path);
        void play_song();
        void play_hitsound(int type);
        void stop();
        void pause();
        void unpause();
    };
    struct MenuAudioManager{
        Mix_Chunk* song;
        const char* song_path;
        MenuAudioManager(const char* song_p);
        void play();
        void stop();
        void pause();
        void unpause();
    };
};