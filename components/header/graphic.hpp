// graphic.hpp
#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include <string>
#include "constants.hpp"
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

    void render(float x, float y, float w, float h);
    void destroy();
};

class MenuButton : public Texture {
public:
    int mouseX, mouseY;
    MenuButton(SDL_Renderer* ren, const char* path);
    bool is_hovering();
};

class Background : public Texture {
public:
    Background(SDL_Renderer* ren, const char* path);
    void render_background();
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

    Text(SDL_Renderer* ren, const char* font_path, int f_size, std::string msg_color, std::string msg);
    ~Text();

    void render(float x, float y, float width, float height, const char* new_message = NULL, std::string new_color = "");
    void set_color(std::string color);
};
