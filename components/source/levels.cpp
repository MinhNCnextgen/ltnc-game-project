// levels.cpp
#include "levels.hpp"

void render_levels_menu(SDL_Renderer* renderer,
                        Background& bg,
                        Text& levels_text,
                        MenuButton& return_button,
                        MenuButton& level1,
                        MenuButton& level2,
                        MenuButton& level3,
                        MenuButton& level4) {
    bg.render_background();
    levels_text.render(Menu::Button::LEVELS_TEXT_X, Menu::Button::LEVELS_TEXT_Y, Menu::Button::LEVELS_TEXT_WIDTH, Menu::Button::LEVELS_TEXT_HEIGHT);
    return_button.render(Menu::Button::RETURN_X, Menu::Button::RETURN_Y, Menu::Button::RETURN_SIZE, Menu::Button::RETURN_SIZE);

    level1.render(Menu::Button::LEVELS_START_X, Menu::Button::LEVELS_START_Y, Menu::Button::LEVEL_WIDTH, Menu::Button::LEVEL_HEIGHT);
    level2.render(Menu::Button::LEVELS_START_X + Menu::Button::LEVEL_WIDTH + Menu::Button::LEVEL_SPACING,
                  Menu::Button::LEVELS_START_Y,
                  Menu::Button::LEVEL_WIDTH, Menu::Button::LEVEL_HEIGHT);

    level3.render(Menu::Button::LEVELS_START_X, Menu::Button::LEVELS_ROW2_Y, Menu::Button::LEVEL_WIDTH, Menu::Button::LEVEL_HEIGHT);
    level4.render(Menu::Button::LEVELS_START_X + Menu::Button::LEVEL_WIDTH + Menu::Button::LEVEL_SPACING,
                  Menu::Button::LEVELS_ROW2_Y,
                  Menu::Button::LEVEL_WIDTH, Menu::Button::LEVEL_HEIGHT);
}
