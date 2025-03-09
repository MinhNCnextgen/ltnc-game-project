#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
using namespace std;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const char* WINDOW_TITLE = "Drum it!";
string game_state = "menu";

void logErrorAndExit(const char* msg, const char* error){
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
    SDL_Quit();
}

SDL_Window* initSDL(int SCREEN_WIDTH, int SCREEN_HEIGHT, const char* WINDOW_TITLE){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logErrorAndExit("SDL_Init", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) logErrorAndExit("CreateWindow", SDL_GetError());

    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)) 
        logErrorAndExit("SDL_image error:", IMG_GetError());

    return window;
}

SDL_Renderer* createRenderer(SDL_Window* window)
{
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) logErrorAndExit("CreateRenderer", SDL_GetError());

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    return renderer;
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

//Texture struct
struct Texture {
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    Texture(SDL_Renderer* ren, const char* path) : renderer(ren), texture(nullptr)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", path);
        texture = IMG_LoadTexture(renderer, path);
        if (texture == NULL)
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());
    }

    ~Texture()
    {
        destroy();
    }

    void render(int x, int y, int w, int h)
    {
        SDL_Rect dest;
        dest.x = x;
        dest.y = y;
        dest.w = w;
        dest.h = h;
        // SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }

    void render_background()
    {
        SDL_RenderCopy(renderer, texture, NULL, NULL);
    }

    void destroy()
    {
        if (texture != nullptr)
        {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }
};

int main(int argc, char* argv[])
{
    //Init
    SDL_Window* window = initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SDL_Renderer* renderer = createRenderer(window);
    //Game texture 
    Texture background(renderer, "assets/menu/background.png");
    Texture play_button(renderer, "assets/menu/Play Button.png");
    Texture quit_button(renderer, "assets/menu/Quit Button.png");
    //Game loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Clear the renderer
        SDL_RenderClear(renderer);
        // Render textures
        if (game_state == "menu"){
            background.render_background();
            play_button.render(390, 100, 500, 200);
            quit_button.render(390, 400, 500, 200);
        }

        // Update renderer
        SDL_RenderPresent(renderer);
    }
    quitSDL(window, renderer);
    return 0;
}