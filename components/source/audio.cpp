#include "audio.hpp"
#include <iostream>

GameAudio::InGameAudioManager::InGameAudioManager(const char* song_path, const char* hitsound_path_left, const char* hitsound_path_right, const char* combobreak_path) {
    song = Mix_LoadMUS(song_path);
    hit_sound_left = Mix_LoadWAV(hitsound_path_left);
    hit_sound_right = Mix_LoadWAV(hitsound_path_right);
    combobreak_sound = Mix_LoadWAV(combobreak_path);
    song_duration = Mix_MusicDuration(song);
    channel = 2;
}

void GameAudio::InGameAudioManager::play_song() {
    Mix_PlayMusic(song, 0);
    Mix_Volume(2, 128 * 70 / 100);
    Mix_Volume(3, 128 * 70 / 100);
    Mix_Volume(4, 128 * 70 / 100);
    Mix_VolumeMusic(128 * 30 / 100);
}

void GameAudio::InGameAudioManager::play_hitsound(int type) {
    if (type == 1) Mix_PlayChannel(2, hit_sound_left, 0);
    if(type == 0) Mix_PlayChannel(3, hit_sound_right,0);
    if (type == 2) Mix_PlayChannel(4, combobreak_sound,0);
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
    if (combobreak_sound) {
        Mix_FreeChunk(combobreak_sound);
        combobreak_sound = nullptr;
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
GameAudio::MenuAudioManager::MenuAudioManager(const char* song_p) : song_path(song_p) {
    song = Mix_LoadWAV(song_path);
}

void GameAudio::MenuAudioManager::play() {
    song = Mix_LoadWAV(song_path);
    Mix_PlayChannel(1, song, -1);
    Mix_Volume(1, 128 * 50 / 100);
}

void GameAudio::MenuAudioManager::stop() {
    Mix_FreeChunk(song);
}

void GameAudio::MenuAudioManager::pause() {
    Mix_HaltChannel(1);
}

void GameAudio::MenuAudioManager::unpause() {
    Mix_Resume(1);
}