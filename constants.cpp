#include "constants.h"

// Window 
const int Screen::WIDTH = 1280;
const int Screen::HEIGHT = 720;
const char* Screen::TITLE = "Drum it!";

// Game timing 
const int GameTiming::FPS = 1000;
const int GameTiming::FRAME_DELAY = 1000 / GameTiming::FPS;

// Score values
const int Game::Note::OK_SCORE = 50;
const int Game::Note::GREAT_SCORE = 100;
const int Game::Note::EXCELLENT_SCORE = 300;
//Note
const int Game::Note::WIDTH = 150;
const int Game::Note::HEIGHT = 150;
const int Game::Note::SPAWN_X = Screen::WIDTH + 300;
const int Game::Note::Y = (Screen::HEIGHT - Game::Note::HEIGHT) / 2;
const int Game::Note::HIT_X = 400;

// Button 
// Main menu
const int Menu::Button::PLAY_X = 390;
const int Menu::Button::PLAY_Y = 100;
const int Menu::Button::PLAY_WIDTH = 500;
const int Menu::Button::PLAY_HEIGHT = 200;

const int Menu::Button::QUIT_X = 390;
const int Menu::Button::QUIT_Y = 400;
const int Menu::Button::QUIT_WIDTH = 500;
const int Menu::Button::QUIT_HEIGHT = 200;

// Levels button
const int Menu::Button::LEVEL_WIDTH = 150;
const int Menu::Button::LEVEL_HEIGHT = 150;
const int Menu::Button::LEVEL_SPACING = 50;
const int Menu::Button::LEVELS_START_X = (Screen::WIDTH - (2 * Menu::Button::LEVEL_WIDTH + Menu::Button::LEVEL_SPACING)) / 2;
const int Menu::Button::LEVELS_START_Y = (Screen::HEIGHT - (2 * Menu::Button::LEVEL_HEIGHT + Menu::Button::LEVEL_SPACING)) / 2;
const int Menu::Button::LEVELS_ROW2_Y = Menu::Button::LEVELS_START_Y + Menu::Button::LEVEL_HEIGHT + Menu::Button::LEVEL_SPACING;

const int Menu::Button::RETURN_X = 10;
const int Menu::Button::RETURN_Y = 10;
const int Menu::Button::RETURN_SIZE = 100;

const int Menu::Button::LEVELS_TEXT_X = (Screen::WIDTH - Menu::Button::LEVELS_TEXT_WIDTH - 325) / 2; 
const int Menu::Button::LEVELS_TEXT_Y = 50;  
const int Menu::Button::LEVELS_TEXT_WIDTH = 200;
const int Menu::Button::LEVELS_TEXT_HEIGHT = 50;

// Game play screen
const int GameUI::Lane::Y = (Screen::HEIGHT - 175) / 2;
const int GameUI::Lane::HEIGHT = 235;

const int GameUI::Drum::X = 15;
const int GameUI::Drum::Y = (Screen::HEIGHT - 125) / 2;
const int GameUI::Drum::SIZE = 150;

const int GameUI::Hitbox::X = 200;
const int GameUI::Hitbox::Y = (Screen::HEIGHT - 150) / 2;
const int GameUI::Hitbox::SIZE = 150;

const int GameUI::Score::X = 10;
const int GameUI::Score::Y = 10;
const int GameUI::Score::WIDTH = 300;
const int GameUI::Score::HEIGHT = 50;

const int GameUI::Streak::X = 10;
const int GameUI::Streak::Y = 70;
const int GameUI::Streak::WIDTH = 50;
const int GameUI::Streak::HEIGHT = 50;

const int GameUI::NoteScore::X = 250;
const int GameUI::NoteScore::Y = (Screen::HEIGHT + 200) / 2;
const int GameUI::NoteScore::WIDTH = 150;
const int GameUI::NoteScore::HEIGHT = 50;

const int GameUI::Health::X = Screen::WIDTH - 300;
const int GameUI::Health::Y = 10;
const int GameUI::Health::WIDTH = 150;
const int GameUI::Health::HEIGHT = 50;

// Game note 
const float Game::Note::MISS_UPPER = 400.0f;
const float Game::Note::MISS_LOWER = -50.0f;
const float Game::Note::OK_UPPER = 300.0f;
const float Game::Note::OK_LOWER = -70.0f;
const float Game::Note::GREAT_UPPER = 200.0f;
const float Game::Note::GREAT_LOWER = -30.0f;
const float Game::Note::EXCELLENT_UPPER = 60.0f;
const float Game::Note::EXCELLENT_LOWER = -30.0f;

// End game screen
const float EndGame::SCOREBOARD_SCALE = 0.9f;
const int EndGame::SCOREBOARD_X = (Screen::WIDTH - (Screen::WIDTH * SCOREBOARD_SCALE)) / 2;
const int EndGame::SCOREBOARD_Y = (Screen::HEIGHT - (Screen::HEIGHT * SCOREBOARD_SCALE)) / 2;
const int EndGame::SCOREBOARD_WIDTH = Screen::WIDTH * SCOREBOARD_SCALE;
const int EndGame::SCOREBOARD_HEIGHT = Screen::HEIGHT * SCOREBOARD_SCALE;

const int EndGame::TEXT_OFFSET_X = 200;
const int EndGame::TEXT_OFFSET_Y = 175;
const int EndGame::TEXT_SPACING = 40;
const int EndGame::TEXT_WIDTH = 500;
const int EndGame::TEXT_HEIGHT = 40;


// Level settings 
const int GameSettings::DEFAULT_HEALTH = 100;
const int GameSettings::LEVEL2_SPEED = 650;
const int GameSettings::LEVEL3_SPEED = 900; 
const int GameSettings::LEVEL4_SPEED = 1200;