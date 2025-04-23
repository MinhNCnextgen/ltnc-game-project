#include "audio.hpp"
#include <iostream>

GameAudio::InGameAudioManager::InGameAudioManager(const char* song_path, const char* hitsound_path_left, const char* hitsound_path_right) {
    song = Mix_LoadMUS(song_path);
    hit_sound_left = Mix_LoadWAV(hitsound_path_left);
    hit_sound_right = Mix_LoadWAV(hitsound_path_right);
    song_duration = Mix_MusicDuration(song);
}

void GameAudio::InGameAudioManager::play_song() {
    Mix_PlayMusic(song, 0);
    Mix_VolumeMusic(128 * 20 / 100);
}

void GameAudio::InGameAudioManager::play_hitsound(int type) {
    if (type == 1) Mix_PlayChannel(1, hit_sound_left, 0);
    if(type == 0) Mix_PlayChannel(1, hit_sound_right,0);
}

void GameAudio::InGameAudioManager::stop() {
    if (hit_sound_right) {
        Mix_FreeChunk(hit_sound_right);
        hit_sound_right = nullptr;
    }
    if (hit_sound_left) {
        Mix_FreeChunk(hit_sound_left);
        hit_sound_left = nullptr;
    }
    if (song) {
        Mix_FreeMusic(song);
        song = nullptr;
    }
}

void GameAudio::InGameAudioManager::pause() {
    Mix_PauseMusic();
    Mix_HaltChannel(-1);
}

void GameAudio::InGameAudioManager::unpause() {
    Mix_ResumeMusic();
    Mix_Resume(-1);
}
GameAudio::MenuAudioManager::MenuAudioManager(const char* song_path, const char* button_click_path) {
    song = Mix_LoadMUS(song_path);
    hit_sound = Mix_LoadWAV(button_click_path);
}

void GameAudio::MenuAudioManager::play_song() {
    Mix_PlayMusic(song, 0);
}

void GameAudio::MenuAudioManager::play_hitsound() {
    Mix_PlayChannel(-1, hit_sound, 0);
}

void GameAudio::MenuAudioManager::stop() {
    Mix_FreeChunk(hit_sound);
    Mix_FreeMusic(song);
}
