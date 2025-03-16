#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
using namespace std;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const char* WINDOW_TITLE = "Drum it!";
string game_state = "main_menu";

void logErrorAndExit(const char* msg, const char* error){
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
    SDL_Quit();
}

SDL_Window* initSDL(int SCREEN_WIDTH, int SCREEN_HEIGHT, const char* WINDOW_TITLE){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) logErrorAndExit("SDL_Init", SDL_GetError());
    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) logErrorAndExit("CreateWindow", SDL_GetError());
    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)) logErrorAndExit("SDL_image error:", IMG_GetError());
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

//Texture class
class Texture {
    public: 
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        SDL_Rect dest;

        Texture(SDL_Renderer* ren, const char* path) : renderer(ren), texture(NULL){
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", path);
            texture = IMG_LoadTexture(renderer, path);
            if (texture == NULL)
                SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());
        }

        ~Texture(){
            destroy();
        }

        void render(int x, int y, int w, int h){
            dest.x = x;
            dest.y = y;
            dest.w = w;
            dest.h = h;
            SDL_RenderCopy(renderer, texture, NULL, &dest);
        }

        void destroy(){
            if (texture != NULL){
                SDL_DestroyTexture(texture);
                texture = NULL;
            }
        }
};
//Menu classes
class MenuButton : public Texture{
    public:
        int mouseX, mouseY;
        const char* type; 
        MenuButton(SDL_Renderer* ren, const char* path, const char* type) : Texture(ren, path), type(type){}
        bool is_hovering(){
            SDL_GetMouseState(&mouseX, &mouseY);
            if (mouseX > dest.x && mouseX < dest.x + dest.w && mouseY > dest.y && mouseY < dest.y + dest.h){
                return true;
            }else{
                return false;
            }
        }
};
class Background : public Texture{
    public:
        Background(SDL_Renderer* ren, const char* path) : Texture(ren, path) {}
        void render_background(string blur)
        {
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128); // Semi-transparent white
            for (int i = 0; i < 10; ++i) {
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderFillRect(renderer, NULL);
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
};
//Level choosing classes
int main(int argc, char* argv[])
{
    //Init
    SDL_Window* window = initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SDL_Renderer* renderer = createRenderer(window);
    //Game texture 
    //Menu - Main screen
    Background background_menu(renderer, "assets/menu/background.png");
    MenuButton play_button(renderer, "assets/menu/Play Button.png", "play");
    MenuButton quit_button(renderer, "assets/menu/Quit Button.png", "quit");
    //Menu - levels
    Texture text_choose_levels(renderer, "assets/menu/choose_levels.png");
    MenuButton level_1(renderer, "assets/menu/lvl1.png", "lvl_1");
    MenuButton level_2(renderer, "assets/menu/lvl2.png", "lvl_2");
    //Game loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type){
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (game_state == "main_menu"){
                        if (play_button.is_hovering()) game_state = "levels_menu";
                        if (quit_button.is_hovering()) running = false;
                    }
                    break;
            }

        }
        // Clear renderer
        SDL_RenderClear(renderer);
        // Render textures
        if (game_state == "main_menu"){
            background_menu.render_background();
            play_button.render(390, 100, 500, 200);
            quit_button.render(390, 400, 500, 200);
        }else if (game_state == "levels_menu"){
            background_menu.render_background();
            text_choose_levels.render(390, 10, 1000, 200);
            level_1.render(100, (SCREEN_HEIGHT - 300) / 2, 300, 300);
            level_2.render(500, (SCREEN_HEIGHT - 300) / 2, 300, 300);
        }
        // Update renderer
        SDL_RenderPresent(renderer);
    }
    quitSDL(window, renderer);
    return 0;
}