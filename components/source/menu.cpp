// menu.cpp
#include "menu.hpp"

void render_main_menu(SDL_Renderer* renderer, Background& bg, MenuButton& play_btn, MenuButton& quit_btn) {
    bg.render_background();
    play_btn.render(Menu::Button::PLAY_X, Menu::Button::PLAY_Y, Menu::Button::PLAY_WIDTH, Menu::Button::PLAY_HEIGHT);
    quit_btn.render(Menu::Button::QUIT_X, Menu::Button::QUIT_Y, Menu::Button::QUIT_WIDTH, Menu::Button::QUIT_HEIGHT);
}
