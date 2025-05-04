// graphic.hpp
#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include <string>
#include "constants.hpp"
#include <functional>

using namespace std;

// Hàm khởi tạo, renderer, quit
void logErrorAndExit(const char* msg, const char* error);
SDL_Window* initSDL(int SCREEN_WIDTH, int SCREEN_HEIGHT, const char* WINDOW_TITLE);
SDL_Renderer* createRenderer(SDL_Window* window);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

class Texture {
public:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect dest;
    const char* path;

    Texture(SDL_Renderer* ren, const char* p);
    ~Texture();

    void render(float x, float y, float w, float h, SDL_Rect* src_rect = NULL);
    void destroy();
};

class Button : public Texture {
    public:
        function<void()> on_click;
        int mouseX, mouseY;
        Button(SDL_Renderer* ren, const char* path, function<void()> callback);
        bool is_hovering();
        void is_click(SDL_Event* event);
};

class Background : public Texture {
public:
    Background(SDL_Renderer* ren, const char* path);
    void render_background();
    void render_background_blur();
};

class Text {
public:
    TTF_Font* font;
    SDL_Color text_color;
    SDL_Surface* text_surface;
    SDL_Renderer* renderer;
    SDL_Texture* text_texture;
    SDL_Rect dest;
    const char* message;
    int font_size;

    Text(SDL_Renderer* ren, const char* font_path, int f_size, string msg_color, string msg);
    ~Text();

    void render(float x, float y, float width, float height, const char* new_message = NULL, string new_color = "");
    void set_color(string color);
};

class Animation : public Texture{
    public:
    vector<SDL_Rect> frames;
    SDL_Rect curr_frame;
    int total_frames, curr_frame_index;
    Uint32 last_frame_time, frame_duration;
    bool freeze;
    Animation(SDL_Renderer* ren, const char* p, vector<SDL_Rect> f, Uint32 f_durr);
    bool is_frame_time();
    void render_next_frame(float x, float y, float width, float height);
};