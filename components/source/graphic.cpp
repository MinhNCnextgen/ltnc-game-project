// graphic.cpp
#include "graphic.hpp"
#include <iostream>

using namespace std;

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

void Texture::render(float x, float y, float w, float h, SDL_Rect* src_rect) {
    dest.x = x;
    dest.y = y;
    dest.w = w;
    dest.h = h;
    SDL_RenderCopy(renderer, texture, src_rect, &dest);
}

void Texture::destroy() {
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
}

Button::Button(SDL_Renderer* ren, const char* path, function<void()> callback) : Texture(ren, path), on_click(callback) {}

bool Button::is_hovering() {
    SDL_GetMouseState(&mouseX, &mouseY);
    if (mouseX > dest.x && mouseX < dest.x + dest.w && mouseY > dest.y && mouseY < dest.y + dest.h){
        return true;
    }else{
        return false;
    }
}

void Button::is_click(SDL_Event* event){
    bool hovered = is_hovering();
    if (event->type == SDL_MOUSEBUTTONDOWN){
        if (hovered){
            on_click();
        }
    }
}

Background::Background(SDL_Renderer* ren, const char* path) : Texture(ren, path) {}

void Background::render_background() {
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}
void Background::render_background_blur() {
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
    SDL_SetTextureAlphaMod(texture, 7); 
    int blurRadius = 2; 
    for (int dx = -blurRadius; dx <= blurRadius; ++dx) {
        for (int dy = -blurRadius; dy <= blurRadius; ++dy) {
            SDL_Rect dest = { dx, dy, Screen::WIDTH, Screen::HEIGHT };
            SDL_RenderCopy(renderer, texture, NULL, &dest);
        }
    }
}

Text::Text(SDL_Renderer* ren, const char* font_path, int f_size, string msg_color, string msg) 
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

void Text::render(float x, float y, float width, float height, const char* new_message, string new_color) {
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

void Text::set_color(string color) {
    if (color == "Black") text_color = {0, 0, 0, 255};
    else if (color == "White") text_color = {255, 255, 255, 255};
    else if (color == "Red") text_color = {255, 0, 0, 255};
    else if (color == "Blue") text_color = {64, 224, 208, 255};
    else if (color == "Yellow") text_color = {255, 255, 0, 255};
    else if (color == "Green") text_color = {0, 255, 0, 255};
    else text_color = {255, 255, 255, 255};
}

Animation::Animation(SDL_Renderer* ren, const char* p, vector<SDL_Rect> f, Uint32 f_durr) : 
    Texture(ren, p),
    frames(f),
    curr_frame(f[0]),
    total_frames(f.size()),
    curr_frame_index(0),
    last_frame_time(SDL_GetTicks()),
    frame_duration(f_durr),
    freeze(false) {}

bool Animation::is_frame_time() {
    Uint32 curr_time = SDL_GetTicks();
    if ((curr_time - last_frame_time) >= frame_duration) {
        last_frame_time = curr_time; 
        return true;
    }
    return false;
}

void Animation::render_next_frame(float x, float y, float width, float height) {
    if (!freeze && is_frame_time()) {
        if (curr_frame_index == total_frames - 1) {
            curr_frame_index = 0;
        } else {
            curr_frame_index++;
        }
        curr_frame = frames[curr_frame_index];
    }
    render(x, y, width, height, &curr_frame);
}