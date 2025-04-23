// graphic.cpp
#include "graphic.hpp"
#include <iostream>

void logErrorAndExit(const char* msg, const char* error) {
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
    SDL_Quit();
    exit(1);
}

SDL_Window* initSDL(int SCREEN_WIDTH, int SCREEN_HEIGHT, const char* WINDOW_TITLE) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logErrorAndExit("SDL_Init", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE,
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (window == nullptr)
        logErrorAndExit("CreateWindow", SDL_GetError());

    if (TTF_Init() == -1)
        logErrorAndExit("TTF_Init", TTF_GetError());

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        logErrorAndExit("Mix_OpenAudio", Mix_GetError());

    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)))
        logErrorAndExit("SDL_image error", IMG_GetError());

    return window;
}

SDL_Renderer* createRenderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                          SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr)
        logErrorAndExit("CreateRenderer", SDL_GetError());

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, Screen::WIDTH, Screen::HEIGHT);

    return renderer;
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer) {
    IMG_Quit();
    Mix_CloseAudio();
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

Texture::Texture(SDL_Renderer* ren, const char* p) : renderer(ren), path(p) {
    texture = IMG_LoadTexture(renderer, path);
    if (texture == NULL) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to load texture %s: %s", path, IMG_GetError());
        exit(1);
    }
}

Texture::~Texture() {
    destroy();
}

void Texture::render(float x, float y, float w, float h) {
    dest.x = x;
    dest.y = y;
    dest.w = w;
    dest.h = h;
    SDL_RenderCopy(renderer, texture, NULL, &dest);
}

void Texture::destroy() {
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
}

MenuButton::MenuButton(SDL_Renderer* ren, const char* path) : Texture(ren, path) {}

bool MenuButton::is_hovering() {
    SDL_GetMouseState(&mouseX, &mouseY);
    return mouseX > dest.x && mouseX < dest.x + dest.w && mouseY > dest.y && mouseY < dest.y + dest.h;
}

Background::Background(SDL_Renderer* ren, const char* path) : Texture(ren, path) {}

void Background::render_background() {
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}

Text::Text(SDL_Renderer* ren, const char* font_path, int f_size, std::string msg_color, std::string msg) 
    : renderer(ren), font_size(f_size), message(msg.c_str()) {
    set_color(msg_color);
    font = TTF_OpenFont(font_path, font_size);
    text_surface = TTF_RenderText_Blended(font, message, text_color);
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
}

Text::~Text() {
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
    TTF_CloseFont(font);
}

void Text::render(float x, float y, float width, float height, const char* new_message, std::string new_color) {
    int textWidth, textHeight;
    SDL_QueryTexture(text_texture, NULL, NULL, &textWidth, &textHeight);
    dest.x = x;
    dest.y = y;
    dest.w = textWidth;
    dest.h = textHeight;

    if (new_message) {
        SDL_DestroyTexture(text_texture);
        SDL_FreeSurface(text_surface);
        message = new_message;
        if (!new_color.empty()) set_color(new_color);
        text_surface = TTF_RenderText_Blended(font, message, text_color);
        text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    }
    SDL_RenderCopy(renderer, text_texture, NULL, &dest);
}

void Text::set_color(std::string color) {
    if (color == "Black") text_color = {0, 0, 0, 255};
    else if (color == "White") text_color = {255, 255, 255, 255};
    else if (color == "Red") text_color = {255, 0, 0, 255};
    else if (color == "Blue") text_color = {64, 224, 208, 255};
    else if (color == "Yellow") text_color = {255, 255, 0, 255};
    else if (color == "Green") text_color = {0, 255, 0, 255};
    else text_color = {255, 255, 255, 255};
}
