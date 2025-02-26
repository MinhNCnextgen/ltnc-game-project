#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
using namespace std;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;
SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;
SDL_Surface* imageSurface = NULL;  

enum KeyPressSurfaces{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_SPACE,
    KEY_PRESS_SURFACE_TOTAL
};

struct MenuButton{
    int x,y,height,width;
    SDL_Surface* img_surface = NULL;
    string img_path;
    MenuButton(int _x, int _y, int _h, int _w, string _path){
        x = _x;
        y = _y;
        height = _h;
        width = _w;
        img_path = _path.c_str();
    }
    void load_image(){
        img_surface = IMG_Load(img_path);
    }
    void on_click(){
        cout << "Button clicked at (" << x << ", " << y << ")" << endl;
    }
};
void init(){
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
    } else {
        window = SDL_CreateWindow("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        } else {
            screenSurface = SDL_GetWindowSurface(window);
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
            SDL_UpdateWindowSurface(window);
        }
    }
}

void loadImage(string imgPath){
    imageSurface = IMG_Load(imgPath.c_str());

}

void close(){
    SDL_FreeSurface(imageSurface);
    imageSurface = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
}

int main(int argc, char *argv[]){
    init();
    loadImage("img/menu/Options Button.png");
    SDL_Event e;
    bool quit = false;
    //Game loop 
    while(!quit){
        //Event queue
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT) quit = true;
        }
        SDL_BlitSurface(imageSurface, NULL, screenSurface, NULL);
        SDL_UpdateWindowSurface(window);
    }
    close();
    return 0;
}