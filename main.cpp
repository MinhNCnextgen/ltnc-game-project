#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include "src/include/json/json.hpp"
#include "constants.hpp"
using namespace std;
using json = nlohmann::json;
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
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)) logErrorAndExit("SDL_image error:", IMG_GetError());
    return window;
}

SDL_Renderer* createRenderer(SDL_Window* window)
{
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) logErrorAndExit("CreateRenderer", SDL_GetError());

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, Screen::WIDTH, Screen::HEIGHT);

    return renderer;
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
    IMG_Quit();
    Mix_CloseAudio();
    TTF_Quit();
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
        MenuButton(SDL_Renderer* ren, const char* path) : Texture(ren, path){}
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
        struct Note {
            float spawn_time;
            float hit_time;
            bool type;
            int index;
            Texture* note_mask;
            static unordered_map <int, Texture*> textures;
            int note_hp;
            float x_pos;
            float y_pos;
            float width;
            float height;
            
            Note(float ht, float st, bool t, int indx, GameManager* gm) :hit_time(ht), spawn_time(st), type(t), index(indx), note_hp(2) {
                note_mask = get_note_texture(t ? "assets/play/note_orange.png" : "assets/play/note_blue.png", (*gm).note_list.size(), gm); // 1 = orange; 0  = blue
                width = Game::Note::WIDTH;
                height = Game::Note::HEIGHT;
                x_pos = Game::Note::SPAWN_X;  
                if (spawn_time < 0){
                    x_pos -= (*gm).game_stats.speed * (-spawn_time / 1000.0f);
                }
                y_pos = Game::Note::Y;
            }
            
            void spawn() {
                note_mask->render(x_pos, y_pos, width, height);
            }
            
            void update_position(float new_x = -1) {
                if (new_x != -1) x_pos = new_x;
                note_mask->render(x_pos, y_pos, width, height);
            }
            void delete_note(GameManager* gm){
                note_mask->destroy();
                note_mask = NULL;
                delete textures[index];
                textures.erase(index);
            }
            
            static Texture* get_note_texture(const char* path, int note_index, GameManager* gm) {
                if (textures.find(note_index) == textures.end()) {
                    textures[note_index] = new Texture(gm->renderer, path);
                }
                return textures[note_index];
            }
        };
        struct TimeManager {
            Uint32 start_time; 
            Uint32 previous_time;
            Uint32 current_time;
            Uint32 elapsed_time;
            Uint32 last_frame_time;
            Uint32 pause_duration; 
            Uint32 pause_start_time; 
            bool is_paused;
        
            TimeManager() : start_time(0), previous_time(0), current_time(0), elapsed_time(0), last_frame_time(0), pause_duration(0), pause_start_time(0), is_paused(false) {}
        
            void update() {
                if (!is_paused) {
                    previous_time = current_time;
                    current_time = SDL_GetTicks() - pause_duration;
                    elapsed_time = current_time - start_time; // Corrected elapsed time calculation
                    last_frame_time = current_time - previous_time;
                    if (last_frame_time > GameTiming::FPS) last_frame_time = GameTiming::FPS;
                }
            }
        
            void start_timer() {
                Uint32 current_ticks = SDL_GetTicks();
                start_time = current_ticks;
                previous_time = current_ticks;
                current_time = current_ticks;
                elapsed_time = 0;
                last_frame_time = 0;
                pause_duration = 0;
                is_paused = false;
            }
        
            void pause() {
                if (!is_paused) {
                    pause_start_time = SDL_GetTicks(); // Record when the pause started
                    is_paused = true;
                }
            }
        
            void unpause() {
                if (is_paused) {
                    pause_duration += SDL_GetTicks() - pause_start_time; // Add the pause duration to the total
                    is_paused = false;
                }
            }
        };
        struct StatsManager{
            int health;
            float speed; //pixel per second
            int point;
            int multiplier;
            float accuracy;
            int excellent_notes;
            int great_notes;
            int ok_notes;
            int missed_notes;
            int highest_streak;
            string final;
            const char* last_note_score;
            StatsManager(int hp, int spd) : health(hp), speed(spd), point(0), multiplier(1), last_note_score(""), excellent_notes(0),
                                      great_notes(0), ok_notes(0), missed_notes(0), accuracy(0), highest_streak(0) {} 
            void stats_update(){
                highest_streak = max(highest_streak, multiplier-1);
                if (last_note_score == "Excellent"){
                    excellent_notes++;
                }else if(last_note_score == "Great"){
                    great_notes++;
                }else if(last_note_score == "Ok"){
                    ok_notes++;
                }else if(last_note_score == "Missed"){
                    missed_notes++;
                }
                float total_notes = excellent_notes + great_notes + ok_notes + missed_notes;
                if (total_notes > 0) {
                    accuracy = ((excellent_notes*300.0f + great_notes*100.0f + ok_notes*50.0f) / (total_notes*300.0f)) * 100.0f;
                } else {
                    accuracy = 0.0f;
                }
            }
        };
        struct KeyManager{
            unordered_map <string, unordered_map<string, int> > key_map;
            KeyManager(){
                key_map = {
                    {"key_blue", {{"time_press",0}, {"time_held", 0}, {"is_holding", 0}}},
                    {"key_orange", {{"time_press",0}, {"time_held", 0}, {"is_holding", 0}}},
                };
            }
            void reset(string key){
                key_map[key]["time_press"] = 0;
                key_map[key]["time_held"] = 0;
                key_map[key]["is_holding"] = 0;
            }
            void key_down(string key){
                unordered_map <string, int>& key_val = key_map.at(key);
                if (key_val.at("is_holding") == 0){
                    key_val.at("is_holding") = 1;
                    if (key_val.at("time_press") == 0) key_val.at("time_press") = SDL_GetTicks();
                }
            }
            void update(){
                for (auto& pair : key_map){
                    if ((pair.first == "key_blue" ? SDL_GetKeyboardState(NULL)[SDL_SCANCODE_F]:SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J])){
                        key_down(pair.first);                    
                        unordered_map <string, int>& key_val = pair.second;
                        if (key_val.at("is_holding") == 1) {
                            key_val.at("time_held") = SDL_GetTicks() - key_val.at("time_press");
                            if ( key_val.at("time_held") > 0) {
                                key_val.at("is_holding") = 0;
                            }
                        }
                    }else{
                        reset(pair.first);
                    }
                }
            }
        };
        struct AudioManager{
            Mix_Music* song;
            Mix_Chunk* hit_sound;
            Uint32 song_duration;
            AudioManager(const char* song_path, const char* hitsound_path){
                cout<<song_path<<endl;
                song = Mix_LoadMUS(song_path);
                hit_sound = Mix_LoadWAV(hitsound_path);
                song_duration = Mix_MusicDuration(song);
            }
            void play_song(){
                Mix_PlayMusic(song, 0);
            }
            void play_hitsound(){
                Mix_PlayChannel(-1, hit_sound, 0);
            }
            void cleanup(){
                Mix_FreeChunk(hit_sound);
                Mix_FreeMusic(song);
            }
            void pause(){
                Mix_PauseMusic();
                Mix_HaltChannel(-1);
            }
            void unpause(){
                Mix_ResumeMusic();
                Mix_Resume(-1);
            }
        };
        KeyManager key_press;
        AudioManager game_audio;
        TimeManager game_time;
        StatsManager game_stats;
        vector <Note> note_list;
        vector <Note*> active_notes;
        int next_note_index;
        bool in_game;
        GameManager(SDL_Renderer* ren, json beatmap) 
            : renderer(ren), 
            game_stats(beatmap["settings"]["default_health"], beatmap["settings"]["default_speed"]), 
            next_note_index(0), 
            game_audio((string("beatmaps/" + string(beatmap["directory"]) + "song.mp3")).c_str(), "sfx/sound/soft-hitnormal.wav"), 
            in_game(true) {
            for(int i=0;i<beatmap["note_list"].size();i++){
                float hit_time = float(beatmap["note_list"][i][1])*1000.0f;
                float distance = Screen::WIDTH + 100.0f;
                float time_needed = (distance / game_stats.speed) * 1000.0f;
                float spawn_time = hit_time - time_needed;
                bool note_type = (beatmap["note_list"][i][0] != 0);
                note_list.emplace_back(hit_time, spawn_time, note_type, i, this);
            }
            
        }
        ~GameManager() {    
            for (auto& pair : Note::textures) {
                delete pair.second;
            }
            game_audio.cleanup();
        }
        void game_update(bool is_paused){
            if (!is_paused){
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
                        if (new_x < 125){
                            (*it)->delete_note(this);
                            it = active_notes.erase(it);
                            game_stats.multiplier = 1;
                            game_stats.health-=1;
                            game_stats.last_note_score = "Missed";
                            game_stats.stats_update();
                        } else {
                            (*it)->update_position(new_x);
                            ++it;
                        }
                    }
                }
                
                // Check if note hit
                key_press.update();
                if (!active_notes.empty()){
                    Note* front_note;
                    float error_gap;
                    for(auto it = active_notes.begin(); it!=active_notes.end(); ++it){
                        front_note = *it;
                        error_gap = front_note->hit_time - game_time.elapsed_time;
                        if (error_gap <= Game::Note::MISS_UPPER && error_gap >= Game::Note::MISS_LOWER && front_note->x_pos <= Game::Note::HIT_X){
                            break;
                        } else {
                            front_note = nullptr;
                            error_gap = 0.0f;
                        }
                    }
                    if(front_note){
                        if ((key_press.key_map["key_blue"]["is_holding"] == 1 && front_note->type == 0) || (key_press.key_map["key_orange"]["is_holding"] == 1 && front_note->type == 1)){
                            // key_press.reset();
                            update_point(error_gap);
                            front_note->delete_note(this);
                            auto it = find(active_notes.begin(), active_notes.end(), front_note);
                            active_notes.erase(it);
                            game_stats.stats_update();
                        }
                        if((key_press.key_map["key_blue"]["is_holding"] == 1 && front_note->type == 1) || (key_press.key_map["key_orange"]["is_holding"] == 1  && front_note->type == 0)){
                            front_note->note_hp -= 1;
                            if (front_note->note_hp<=1){
                                // key_press.reset();
                                auto it = find(active_notes.begin(), active_notes.end(), front_note);
                                it = active_notes.erase(it);
                                game_stats.multiplier = 1;
                                game_stats.health-=1;
                                game_stats.last_note_score = "Missed";
                                game_stats.stats_update();            
                                

                            }
                        }
                    }
                }
                // Check game conditions
                if (game_stats.health <= 0){
                    game_stats.final = "lose";
                    game_audio.cleanup();
                    in_game = false;
                }
                if(game_time.elapsed_time > game_audio.song_duration*1000.0f+3000.0f){
                    game_stats.final = "win";
                    game_audio.cleanup();
                    in_game = false;
                }
            }else{
                for (auto it = active_notes.begin(); it != active_notes.end();) { 
                    (*it)->update_position();
                    ++it;
                }
            }
        }
        void update_point(float gap){
            if (gap <= Game::Note::MISS_UPPER && gap >= Game::Note::OK_UPPER){
                game_stats.multiplier = 1;
                game_stats.health-=1;
                game_stats.last_note_score = "Missed";
            }else if(gap < Game::Note::OK_UPPER && gap >= Game::Note::GREAT_UPPER){
                game_stats.point += Game::Note::OK_SCORE * game_stats.multiplier;
                game_stats.multiplier++;
                game_stats.last_note_score = "Ok";
                game_audio.play_hitsound();
            }else if(gap < Game::Note::GREAT_UPPER && gap >= Game::Note::EXCELLENT_UPPER){
                game_stats.point += Game::Note::GREAT_SCORE * game_stats.multiplier;
                game_stats.multiplier++;
                game_stats.last_note_score = "Great";
                game_audio.play_hitsound();
            }else if(gap < Game::Note::EXCELLENT_UPPER && gap >= Game::Note::EXCELLENT_LOWER){
                game_stats.point += Game::Note::EXCELLENT_SCORE * game_stats.multiplier;
                game_stats.multiplier++;
                game_stats.last_note_score = "Excellent";
                game_audio.play_hitsound();
            }else if (gap < Game::Note::EXCELLENT_LOWER && gap >= Game::Note::GREAT_LOWER){
                game_stats.point += Game::Note::GREAT_SCORE * game_stats.multiplier;
                game_stats.multiplier++;
                game_stats.last_note_score = "Great";
                game_audio.play_hitsound();
            }else if (gap < Game::Note::GREAT_LOWER && gap >= Game::Note::OK_LOWER){
                game_stats.point += Game::Note::OK_SCORE * game_stats.multiplier;
                game_stats.multiplier++;
                game_stats.last_note_score = "Ok";
                game_audio.play_hitsound();
            }else{
                game_stats.multiplier = 1;
                game_stats.health-=1;
                game_stats.last_note_score = "Missed";
            }
        }
        
        unordered_map <string, int> get_final_stats(){
            return {
                {
                    {"type", game_stats.final == "win" ? 1 : 0},
                    {"score", game_stats.point},
                    {"accuracy", game_stats.accuracy},
                    {"highest_streak", game_stats.highest_streak},
                    {"excellent_notes", game_stats.excellent_notes},
                    {"great_notes", game_stats.great_notes},
                    {"ok_notes", game_stats.ok_notes},
                    {"missed_notes", game_stats.missed_notes}
                }
            };
        }
};
unordered_map<int, Texture*> GameManager::Note::textures;

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
            set_color(msg_color);
            font = TTF_OpenFont(font_path, font_size);
            text_surface = TTF_RenderText_Blended(font, message, text_color);
            text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        }
        void render(float x, float y, float width, float height,const char* new_message = NULL, string new_color = ""){
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
                if (!new_color.empty()){
                    set_color(new_color);
                }
                text_surface = TTF_RenderText_Blended(font, message, text_color);
                text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            }
            SDL_RenderCopy(renderer, text_texture, NULL, &dest);
        }
        void set_color(string color){
            if (color == "Black") {
                text_color = {0, 0, 0, 255}; 
            } else if (color == "White") {
                text_color = {255, 255, 255, 255};
            } else if (color == "Red"){
                text_color = {255, 0, 0, 255};
            } else if (color == "Blue"){
                text_color = {64, 224, 208, 255};
            }else if (color == "Yellow"){
                text_color = {255, 255, 0, 255};
            }else if (color == "Green"){
                text_color = {0, 255, 0, 255};
            }else{
                text_color = {255, 255, 255, 255};
            }
        }
        ~Text(){
            SDL_DestroyTexture(text_texture);
            SDL_FreeSurface(text_surface);
            TTF_CloseFont(font);
        }
};
void initialize_level(GameManager*& game, SDL_Renderer* renderer, string beatmap_path) {
    // Construct the file path
    string file_path = "beatmaps/" + beatmap_path + "/data.json";
    ifstream file(file_path);
    json beatmap;
    file >> beatmap;
    // Initialize the game
    game = new GameManager(renderer, beatmap);
    game->game_time.start_timer();
    game->game_audio.play_song();
    game->in_game = true;
    file.close();
}

//Main
int main(int argc, char* argv[])
{
    //Init
    SDL_Window* window = initSDL(Screen::WIDTH, Screen::HEIGHT, Screen::TITLE);
    SDL_Renderer* renderer = createRenderer(window);
    //Game texture 
    //Menu - Main screen
    Background background_menu(renderer, "assets/menu/background.png");
    MenuButton play_button(renderer, "assets/menu/Play Button.png");
    MenuButton quit_button(renderer, "assets/menu/Quit Button.png");
    //Menu - levels
    Text text_choose_levels(renderer, "font/Aller_bd.ttf", 84, "Blue", "SELECT LEVEL");
    MenuButton level_1(renderer, "assets/menu/lvl1.png");
    MenuButton level_2(renderer, "assets/menu/lvl2.png");
    MenuButton level_3(renderer, "assets/menu/lvl3.png");
    MenuButton level_4(renderer, "assets/menu/lvl4.png");
    //Play
    Background background_game(renderer, "assets/play/game_bg.png");
    Texture drum(renderer, "assets/play/drum.png");
    Texture drum_left(renderer, "assets/play/drum_left.png");
    Texture drum_right(renderer, "assets/play/drum_right.png");
    Texture hit_box(renderer, "assets/play/hitbox.png");
    Texture lane(renderer, "assets/play/lane.png");
    Text score(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "White", "");
    Text streak(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "White", "");
    Text note_score(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "White", "");
    Text health(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "White", "");
    Text accuracy(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "White", "");
    Text game_time(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "White", "");    
    GameManager* game = nullptr; 
    unordered_map <string, int> game_output;
    bool paused = false;
    //End game
    MenuButton return_to_screen(renderer, "assets/end_screen/back.png");
    Texture score_board(renderer, "assets/end_screen/scoreboard.png");
    Text final_score(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text game_result(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text final_acc(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text highest_streak(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text great_notes(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text excellent_notes(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text ok_notes(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");
    Text missed_notes(renderer, "font/Aller_bd.ttf", GameUI::TEXT_SIZE, "Black", "");

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
                if (level_1.is_hovering()){
                    screen_state = "play";
                    initialize_level(game, renderer, "map1");
                }
                else if (level_2.is_hovering()) {
                    screen_state = "play";
                    initialize_level(game, renderer, "map2");
                }else if(level_3.is_hovering()){
                    screen_state = "play";
                    initialize_level(game, renderer, "map3");
                }else if(level_4.is_hovering()){
                    screen_state = "play";
                    initialize_level(game, renderer, "map4");
                }else if(return_to_screen.is_hovering()){
                    screen_state = "main_menu";
                }
                }else if(screen_state == "end_game"){
                if (return_to_screen.is_hovering()){
                    screen_state = "levels_menu";
                }
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    if (screen_state == "play") {
                        if (!paused){
                            paused = true;
                            game->game_time.pause();
                            game->game_audio.pause();
                        }else{
                            paused = false;
                            game->game_time.unpause();
                            game->game_audio.unpause();
                        }
                    } else if (screen_state == "levels_menu" ) {
                        screen_state = "main_menu";
                    } else if (screen_state == "end_game"){
                        screen_state = "levels_menu"
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
            play_button.render(Menu::Button::PLAY_X, Menu::Button::PLAY_Y, Menu::Button::PLAY_WIDTH, Menu::Button::PLAY_HEIGHT);
            quit_button.render(Menu::Button::QUIT_X, Menu::Button::QUIT_Y, Menu::Button::QUIT_WIDTH, Menu::Button::QUIT_HEIGHT);

        }else if (screen_state == "levels_menu"){
            background_menu.render_background();
            text_choose_levels.render(Menu::Button::LEVELS_TEXT_X, Menu::Button::LEVELS_TEXT_Y, Menu::Button::LEVELS_TEXT_WIDTH, Menu::Button::LEVELS_TEXT_HEIGHT);
            return_to_screen.render(Menu::Button::RETURN_X, Menu::Button::RETURN_Y, Menu::Button::RETURN_SIZE, Menu::Button::RETURN_SIZE);
            level_1.render(Menu::Button::LEVELS_START_X, Menu::Button::LEVELS_START_Y, Menu::Button::LEVEL_WIDTH, Menu::Button::LEVEL_HEIGHT);
            level_2.render(Menu::Button::LEVELS_START_X + Menu::Button::LEVEL_WIDTH + Menu::Button::LEVEL_SPACING, Menu::Button::LEVELS_START_Y, Menu::Button::LEVEL_WIDTH, Menu::Button::LEVEL_HEIGHT);
            level_3.render(Menu::Button::LEVELS_START_X, Menu::Button::LEVELS_ROW2_Y, Menu::Button::LEVEL_WIDTH, Menu::Button::LEVEL_HEIGHT);
            level_4.render(Menu::Button::LEVELS_START_X + Menu::Button::LEVEL_WIDTH + Menu::Button::LEVEL_SPACING, Menu::Button::LEVELS_ROW2_Y, Menu::Button::LEVEL_WIDTH, Menu::Button::LEVEL_HEIGHT);
        }
        else if (screen_state == "play"){
            if (game->in_game){        
                background_game.render_background();
                lane.render(0, GameUI::Lane::Y, Screen::WIDTH, GameUI::Lane::HEIGHT);
                hit_box.render(GameUI::Hitbox::X, GameUI::Hitbox::Y, GameUI::Hitbox::SIZE, GameUI::Hitbox::SIZE);
                drum.render(GameUI::Drum::X, GameUI::Drum::Y, GameUI::Drum::SIZE, GameUI::Drum::SIZE);
                if (!paused){
                    game->game_update(paused);
                    if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_F]) {
                        drum_left.render(GameUI::Drum::X, GameUI::Drum::Y, GameUI::Drum::SIZE, GameUI::Drum::SIZE);
                    }
                    if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J]) {
                        drum_right.render(GameUI::Drum::X, GameUI::Drum::Y, GameUI::Drum::SIZE, GameUI::Drum::SIZE);
                    }
                }else{
                    game->game_update(paused);
                }
                score.render(GameUI::Score::X, GameUI::Score::Y, GameUI::Score::WIDTH, GameUI::Score::HEIGHT, ("Score: " + to_string(game->game_stats.point)).c_str());
                streak.render(GameUI::Streak::X, GameUI::Streak::Y, GameUI::Streak::WIDTH, GameUI::Streak::HEIGHT, ("x" + to_string(game->game_stats.multiplier-1)).c_str());
                accuracy.render(GameUI::Accuracy::X, GameUI::Accuracy::Y, GameUI::Accuracy::WIDTH, GameUI::Accuracy::HEIGHT, ("Accuracy: " + to_string(int(round(game->game_stats.accuracy))) + "%").c_str());
                game_time.render(GameUI::Time::X, GameUI::Time::Y, GameUI::Time::WIDTH, GameUI::Time::HEIGHT, ("Time Left: " + to_string(((game->game_audio.song_duration+3) * 1000 - game->game_time.elapsed_time) / 1000) + "s").c_str());
                note_score.render(GameUI::NoteScore::X, GameUI::NoteScore::Y, GameUI::NoteScore::WIDTH, GameUI::NoteScore::HEIGHT, 
                    (game->game_stats.last_note_score), 
                    (string(game->game_stats.last_note_score) == "Excellent" ? "Blue" : 
                     string(game->game_stats.last_note_score) == "Great" ? "Green" : 
                     string(game->game_stats.last_note_score) == "Ok" ? "Yellow" : "Red")
                );
                health.render(GameUI::Health::X, GameUI::Health::Y, GameUI::Health::WIDTH, GameUI::Health::HEIGHT, ("Health: " + to_string(game->game_stats.health)).c_str());
            }else{
                game_output = game->get_final_stats();
                screen_state = "end_game";
                if (!game){
                    delete game;
                    game = nullptr;
                }
            }
        }else if (screen_state == "end_game"){
            background_menu.render_background();
            score_board.render(EndGame::SCOREBOARD_X, EndGame::SCOREBOARD_Y, EndGame::SCOREBOARD_WIDTH, EndGame::SCOREBOARD_HEIGHT);
            return_to_screen.render(Menu::Button::RETURN_X, Menu::Button::RETURN_Y, Menu::Button::RETURN_SIZE, Menu::Button::RETURN_SIZE);
            int base_x = EndGame::SCOREBOARD_X + EndGame::TEXT_OFFSET_X;
            int base_y = EndGame::SCOREBOARD_Y + EndGame::TEXT_OFFSET_Y;
            final_score.render(base_x, base_y, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT, ("Final Score: " + to_string(game_output["score"])).c_str());
            game_result.render(base_x, base_y + EndGame::TEXT_SPACING, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT, ("Result: " + string(game_output["type"] == 1 ? "Pass" : "Fail")).c_str());
            final_acc.render(base_x, base_y + EndGame::TEXT_SPACING * 2, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT, ("Accuracy: " + to_string(game_output["accuracy"]) + "%").c_str());
            highest_streak.render(base_x, base_y + EndGame::TEXT_SPACING * 3, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT, ("Highest Streak: " + to_string(game_output["highest_streak"])).c_str());
            excellent_notes.render(base_x, base_y + EndGame::TEXT_SPACING * 4, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT, ("Excellent Notes: " + to_string(game_output["excellent_notes"])).c_str());
            great_notes.render(base_x, base_y + EndGame::TEXT_SPACING * 5, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT, ("Great Notes: " + to_string(game_output["great_notes"])).c_str());
            ok_notes.render(base_x, base_y + EndGame::TEXT_SPACING * 6, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT, ("OK Notes: " + to_string(game_output["ok_notes"])).c_str());
            missed_notes.render(base_x, base_y + EndGame::TEXT_SPACING * 7, EndGame::TEXT_WIDTH, EndGame::TEXT_HEIGHT, ("Missed Notes: " + to_string(game_output["missed_notes"])).c_str());
        }
        // Update renderer 
        SDL_RenderPresent(renderer);
        //Limit frame speed
        frame_time = SDL_GetTicks() - frame_start;
        if (GameTiming::FRAME_DELAY > frame_time){
            SDL_Delay(GameTiming::FRAME_DELAY - frame_time);
        }
    }
    // Clean up 
    if (game != nullptr) {
        delete game;
    }
    quitSDL(window, renderer);
    return 0;
}

