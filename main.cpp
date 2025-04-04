#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
#include <fstream>
#include <vector>
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
    TTF_Init();
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
            // SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", path);
            texture = IMG_LoadTexture(renderer, path);
            if (texture == NULL) {
                SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to load texture %s: %s", path, IMG_GetError());
                exit(1);  // Exit if texture loading fails
            }
            // SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Successfully loaded texture %s", path);
        }
        ~Texture(){
            destroy();
        }
        void render(float x, float y, float w, float h){
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
        struct Note {
            float spawn_time;
            float hit_time;
            bool type;
            int index;
            Texture* note_mask;
            float x_pos;
            float y_pos;
            float width;
            float height;
            
            Note(float ht, float st, bool t, int indx, GameManager* gm) :hit_time(ht), spawn_time(st), type(t), index(indx) {
                note_mask = (*gm).get_note_texture(t ? "assets/play/note_orange.png" : "assets/play/note_blue.png", (*gm).note_list.size());
                width = 150;
                height = 150;
                x_pos = SCREEN_WIDTH + 300;  
                if (spawn_time < 0){
                    x_pos -= (*gm).game_stats.speed * (-spawn_time / 1000.0f);
                }
                y_pos = (SCREEN_HEIGHT - height) / 2;
            }
            
            void spawn() {
                (*note_mask).render(x_pos, y_pos, width, height);
            }
            
            void update_position(float new_x) {
                x_pos = new_x;
                (*note_mask).render(x_pos, y_pos, width, height);
            }
            void delete_note(GameManager* gm){
                note_mask = NULL;
                delete gm->textures[index];
                gm->textures.erase(index);
            }
        };
        struct TimeManager{
            Uint32 start_time; // milisecond 
            Uint32 previous_time;
            Uint32 current_time; 
            Uint32 elapsed_time;
            Uint32 last_frame_time;
            void update() {
                previous_time = current_time; 
                current_time = SDL_GetTicks();
                elapsed_time = current_time - start_time;
                last_frame_time = current_time - previous_time;  
                if (last_frame_time > FPS) last_frame_time = FPS;
            }
            void start_timer() {
                Uint32 current_ticks = SDL_GetTicks();
                start_time = current_ticks;
                previous_time = current_ticks;
                current_time = current_ticks;
                elapsed_time = 0;
                last_frame_time = 0;
            }
        };
        struct StatsManager{
            int health;
            float speed; //pixel per second
            int point;
            int multiplier;
            const char* last_note_score;
            StatsManager(float spd) : health(5), speed(spd), point(0), multiplier(1), last_note_score("") {} 

        };
        struct KeyManager{
            bool key_blue;
            bool key_orange;
            bool is_holding;
            Uint32 press_time;
            Uint32 time_held;
            KeyManager(){
                reset();
            }
            void reset(){
                key_blue = 0;
                key_orange = 0;
                is_holding = false;
                time_held = 0;
                press_time = 0;
            }
            void key_down_blue(){
                if (!is_holding) press_time = SDL_GetTicks();
                if (time_held == 0) key_blue = 1;
                else key_blue = 0;
                is_holding = true;
            }
            void key_down_orange(){
                if (!is_holding) press_time = SDL_GetTicks();
                if (time_held == 0) key_orange = 1;
                else key_orange = 0;
                is_holding = true;
            }
            void key_update(){
                if (is_holding) {
                    time_held = SDL_GetTicks() - press_time;
                    if (time_held > 0) {
                        key_blue = 0;
                        key_orange = 0;
                    }
                }
            }
        };
        KeyManager key_press;
        TimeManager game_time;
        StatsManager game_stats;
        vector <Note> note_list;
        vector <Note*> active_notes;
        size_t next_note_index;
        GameManager(SDL_Renderer* ren, string data_path, string music_path) 
            : renderer(ren), game_stats(350), next_note_index(0) {
            ifstream file(data_path);
            if (!file.is_open()) {
                cout << "Failed to open data file!" << endl;
                exit(1);
            }
            string line;
            int count = 0;
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
                float hit_time = stof(time)*1000.0f;
                float distance = SCREEN_WIDTH + 100.0f;
                float time_needed = (distance / game_stats.speed) * 1000.0f;
                float spawn_time = hit_time - time_needed;
                note_list.emplace_back(hit_time, spawn_time, type, count, this); 
                count++;
            }
            file.close();
            game_time.start_timer();
        }
        ~GameManager() {
            for (auto& pair : textures) {
                delete pair.second;
            }
        }
        Texture* get_note_texture(const char* path, int note_index) {
            if (textures.find(note_index) == textures.end()) {
                textures[note_index] = new Texture(renderer, path);
            }
            return textures[note_index];
        }
        void game_update(){
            game_time.update();
            // Spawn new notes
            if (!note_list.empty() && next_note_index < note_list.size()){
                Note& curr_note = note_list[next_note_index];
                if (game_time.elapsed_time >= curr_note.spawn_time) {
                    curr_note.spawn();
                    active_notes.push_back(&curr_note);
                    next_note_index++;
                }
            }
            // Update existing notes
            if (!active_notes.empty()){
                float new_x;
                for (auto it = active_notes.begin(); it != active_notes.end();) { 
                    new_x = (*it)->x_pos - (game_stats.speed * (float(game_time.last_frame_time) / 1000.0f));
                    if (new_x < -200){
                        (*it)->delete_note(this);
                        it = active_notes.erase(it);
                        game_stats.multiplier = 1;
                        game_stats.health-=1;
                        game_stats.last_note_score = "Missed";
                    } else {
                        (*it)->update_position(new_x);
                        ++it;
                    }
                }
            }
            // Check if note hit
            if (!active_notes.empty()){
                Note* front_note = active_notes.front();
                float error_gap = front_note->hit_time - game_time.elapsed_time;
                if (key_press.key_blue && front_note->type == 0){
                    if(error_gap <= 500){
                        update_point(error_gap);
                        cout<<"Note blue hit!"<<endl;
                        front_note->delete_note(this);
                        active_notes.erase(active_notes.begin());
                    }
                }else if (key_press.key_orange && front_note->type == 1){
                    if(error_gap <= 500){
                        update_point(error_gap);
                        cout<<"Note orange hit!"<<endl;
                        front_note->delete_note(this);
                        active_notes.erase(active_notes.begin());
                    }
                }
            }
            key_press.key_update();
        }
        void update_point(float gap){
            if (gap<=500 && gap >= 300){
                game_stats.multiplier = 1;
                game_stats.health-=1;
                game_stats.last_note_score = "Missed";
            }else if(gap<300 && gap>=200){
                game_stats.point+= 50 * game_stats.multiplier;
                game_stats.multiplier++;
                game_stats.last_note_score = "Ok";
            }else if(gap<200 && gap>=60){
                game_stats.point+= 100 * game_stats.multiplier;
                game_stats.multiplier++;
                game_stats.last_note_score = "Great";
            }else if(gap<60 && gap>=-30){
                game_stats.point+= 300 * game_stats.multiplier;
                game_stats.multiplier++;
                game_stats.last_note_score = "Excellent";
            }else if (gap<-30 && gap>=-70){
                game_stats.point+= 100 * game_stats.multiplier;
                game_stats.multiplier++;
                game_stats.last_note_score = "Great";
            }else if (gap<-70 && gap>=-100){
                game_stats.point+= 50 * game_stats.multiplier;
                game_stats.multiplier++;
                game_stats.last_note_score = "Ok";
            }else{
                game_stats.multiplier = 1;
                game_stats.health-=1;
                game_stats.last_note_score = "Missed";
            }
        }
};
class Text{
    public:
        TTF_Font* font;
        SDL_Color text_color;
        SDL_Surface* text_surface;
        SDL_Renderer* renderer;
        SDL_Texture* text_texture;
        SDL_Rect dest;
        const char* message;
        int font_size;
        Text(SDL_Renderer* ren, const char* font_path, int f_size, string msg_color, string msg) : renderer(ren), message(msg.c_str()), font_size(f_size){
            if (msg_color == "Black") {
                text_color = {0, 0, 0, 255}; 
            } else if (msg_color == "White") {
                text_color = {255, 255, 255, 255};
            } else {
                text_color = {255, 0, 0, 255};
            }
            font = TTF_OpenFont(font_path, font_size);
            text_surface = TTF_RenderText_Blended(font, message, text_color);
            text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        }
        void render(float x, float y, float width, float height,const char* new_message){
            int textWidth, textHeight;
            SDL_QueryTexture(text_texture, NULL, NULL, &textWidth, &textHeight);
            dest.x = x;
            dest.y = y;
            dest.w = textWidth;
            dest.h = textHeight;
            if (new_message){
                SDL_DestroyTexture(text_texture);
                SDL_FreeSurface(text_surface);
                message = new_message;
                text_surface = TTF_RenderText_Blended(font, message, text_color);
                text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            }
            SDL_RenderCopy(renderer, text_texture, NULL, &dest);
        }
        ~Text(){
            SDL_DestroyTexture(text_texture);
            SDL_FreeSurface(text_surface);
            TTF_CloseFont(font);
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
    Texture drum(renderer, "assets/play/drum.png");
    Texture hit_box(renderer, "assets/play/hitbox.png");
    Texture lane(renderer, "assets/play/lane.png");
    Text score(renderer, "font/Aller_bd.ttf", 54, "Black", "");
    Text streak(renderer, "font/Aller_bd.ttf", 54, "Black", "");
    Text note_score(renderer, "font/Aller_bd.ttf", 34, "Black", "");
    Text health(renderer, "font/Aller_bd.ttf", 54, "Black", "");
    GameManager* game = nullptr; 
    //Game loop
    bool running = true;
    bool in_game = false;
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
                        if (level_1.is_hovering()) {
                            screen_state = "play";
                            if (game != nullptr) delete game;
                            game = new GameManager(renderer, "data/1.txt", "sfx/lvl1.mp3");
                            game->game_time.start_timer();
                            in_game = true;
                        }
                    }
                    break;
                case SDL_KEYDOWN:
                    if (screen_state == "play" && game){
                        switch (event.key.keysym.sym)
                        {
                        case SDLK_f:
                            game->key_press.key_down_blue();
                            break;
                        case SDLK_j:
                            game->key_press.key_down_orange();
                            break;
                        }
                    }
                    break;
                case SDL_KEYUP:
                    if (screen_state == "play" && game){
                        switch (event.key.keysym.sym)
                        {
                        case SDLK_f:
                            game->key_press.reset();
                            break;
                        case SDLK_j:
                            game->key_press.reset();
                            break;
                        }
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
            lane.render(0, (SCREEN_HEIGHT - 175) / 2, SCREEN_WIDTH, 235);
            drum.render(50, (SCREEN_HEIGHT - 150) / 2, 150, 150);
            hit_box.render(200,(SCREEN_HEIGHT - 150) / 2,150,150);
            game->game_update();
            score.render(10, 10, 300, 50, ("Score: " + to_string(game->game_stats.point)).c_str());
            streak.render(10, 70, 50, 50, ("x" + to_string(game->game_stats.multiplier-1)).c_str());
            note_score.render(10, 130, 150, 50, (game->game_stats.last_note_score));
            health.render(SCREEN_WIDTH - 300, 10, 150, 50, ("Health: " + to_string(game->game_stats.health)).c_str());
        }
        // Update renderer
        SDL_RenderPresent(renderer);
        //Limit frame speed
        frame_time = SDL_GetTicks() - frame_start;
        if (frame_delay > frame_time){
            SDL_Delay(frame_delay - frame_time);
        }
    }
    // Clean up 
    if (game != nullptr) {
        delete game;
    }
    quitSDL(window, renderer);
    return 0;
}
