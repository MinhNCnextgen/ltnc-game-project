#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <unordered_map>
#include <fstream>
#include <queue>
#include <string>
using namespace std;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const char* WINDOW_TITLE = "Drum it!";
const Uint32 FPS = 60;
const Uint32 frame_delay = 1000/FPS;
string screen_state = "main_menu";

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
        const char* path;
        Texture(SDL_Renderer* ren, const char* p){
            path = p;
            renderer = ren;
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", path);
            texture = IMG_LoadTexture(renderer, path);
            if (texture == NULL) {
                SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to load texture %s: %s", path, IMG_GetError());
                exit(1);  // Exit if texture loading fails
            }
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Successfully loaded texture %s", path);
        }
        ~Texture(){
            destroy();
        }
        void render(double x, double y, double w, double h){
            dest.x = x;
            dest.y = y;
            dest.w = w;
            dest.h = h;
            if (texture == NULL) return;
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
        void render_background()
        {
            SDL_RenderCopy(renderer, texture, NULL, NULL);
        }
};
//Game class
class GameManager{
    public:
        SDL_Renderer* renderer;
        unordered_map <int, Texture*> textures;
        struct note {
            double spawn_time;
            bool type;
            Texture* note_mask;          
            note(double st, bool t, GameManager* gm) : spawn_time(st), type(t) {
                note_mask = (*gm).getTexture(t ? "assets/play/note_blue.png" : "assets/play/note_orange.png", (*gm).note_list.size());
            }
            
            void spawn() {
                double x = SCREEN_WIDTH - 100; 
                double y = (SCREEN_HEIGHT - 100) / 2; 
                (*note_mask).render(x, y, 100, 100); 
            }
            void update_position(double new_x){
                (*note_mask).render(new_x, (*note_mask).dest.y, (*note_mask).dest.w, (*note_mask).dest.h);
            }
        };
        struct TimeManager{
            Uint32 start_time; // milisecond 
            Uint32 current_time; 
            Uint32 elapsed_time;
            Uint32 last_frame_time;
            void update() {
                current_time = SDL_GetTicks();
                last_frame_time = current_time - elapsed_time;
                elapsed_time = current_time - start_time;
            }
        };
        struct StatsManager{
            int health;
            int speed; //pixel per second
            int point;
            StatsManager(int hp, int spd, int p) : health(hp), speed(spd), point(p) {}
        };
        TimeManager game_time;
        StatsManager game_stats;
        vector <note> note_list;
        vector <note*> active_notes;
        GameManager(SDL_Renderer* ren, string data_path, string music_path) 
            : renderer(ren), game_stats(100, 1, 0), game_time{SDL_GetTicks(), 0, 0} {
            ifstream file(data_path);
            if (!file.is_open()) {
                cout << "Failed to open data file!" << endl;
                exit(1);
            }
            string line;
            while (getline(file, line)){
                string time = "";
                bool type;
                for (int i=0;i<line.size();i++){
                    if (i==0){
                        type = line[0] - '0';
                    }
                    if(i>=2){
                        time+=line[i];
                    }
                }
                note_list.emplace_back(double(stoi(time) * 1000), type, this);
            }
            file.close();
            game_time.start_time = SDL_GetTicks();
            game_time.update();
        }
        ~GameManager() {
            for (auto& pair : textures) {
                delete pair.second;
            }
        }
        Texture* getTexture(const char* path, int note_index) {
            if (textures.find(note_index) == textures.end()) {
                textures[note_index] = new Texture(renderer, path);
            }
            return textures[note_index];
        }
        void game_update(){
            game_time.update();
            if (!note_list.empty()){
                note& curr_note = note_list.at(active_notes.size());
                if (game_time.elapsed_time>=curr_note.spawn_time){
                    curr_note.spawn();
                    active_notes.push_back(&curr_note);
                }
            }
            if (!active_notes.empty()){
                for (note* note_ptr : active_notes) {
                    double x_pos = (*(*note_ptr).note_mask).dest.x - (game_stats.speed*(game_time.last_frame_time/100));
                    (*note_ptr).update_position(x_pos);
                }
            }
        }
};

//Main
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
    //Play 
    GameManager game(renderer, "data/1.txt", "sfx/lvl1.mp3");
    cout << "First note type: " << game.note_list[0].type << endl;
    //Game loop
    bool running = true;
    Uint32 frame_start, frame_time;
    SDL_Event event;
    while (running) {
        frame_start = SDL_GetTicks();
        while (SDL_PollEvent(&event)) {
            switch (event.type){
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (screen_state == "main_menu"){
                        if (play_button.is_hovering()) screen_state = "levels_menu";
                        if (quit_button.is_hovering()) running = false;
                    }else if(screen_state == "levels_menu"){
                        if (level_1.is_hovering()) screen_state = "play";
                    }
                    break;
            }

        }
        // Clear renderer
        SDL_RenderClear(renderer);
        // Render textures
        if (screen_state == "main_menu"){
            background_menu.render_background();
            play_button.render(390, 100, 500, 200);
            quit_button.render(390, 400, 500, 200);
        }else if (screen_state == "levels_menu"){
            background_menu.render_background();
            text_choose_levels.render((SCREEN_WIDTH - 400) / 2, 50, 400, 100);
            level_1.render((SCREEN_WIDTH - (2 * 100 + 200)) / 2, (SCREEN_HEIGHT - 110) / 2, 100, 110);
            level_2.render((SCREEN_WIDTH - (2 * 100 + 200)) / 2 + 100 + 200, (SCREEN_HEIGHT - 110) / 2, 100, 110);
        }
        else if (screen_state == "play"){
            background_menu.render_background();
            game.game_update();
        }
        // Update renderer
        SDL_RenderPresent(renderer);
        frame_time = SDL_GetTicks() - frame_start;
        if (frame_delay > frame_time){
            SDL_Delay(frame_delay - frame_time);
        }
    }
    quitSDL(window, renderer);
    return 0;
}