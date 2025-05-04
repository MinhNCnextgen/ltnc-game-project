#include "../header/constants.hpp"

// Window
namespace Screen {
    const int WIDTH = 1280;
    const int HEIGHT = 720;
    const char* TITLE = "Drum it!";
}

// Game timing
namespace GameTiming {
    const int FPS = 240;
    const int FRAME_DELAY = 1000 / FPS;
}

// Score values
namespace Game {
    namespace Note {
        const int OK_SCORE = 50;
        const int GREAT_SCORE = 100;
        const int EXCELLENT_SCORE = 300;

        // Note
        const int WIDTH = 150;
        const int HEIGHT = 150;
        const int SPAWN_X = Screen::WIDTH + 300;
        const int Y = (Screen::HEIGHT - HEIGHT) / 2;
        const int HIT_X = 400;
        const int DEATH_X = 125;
        // Game note
        const float MISS_UPPER = 200.0f;
        const float MISS_LOWER = -200.0f;
        const float OK_UPPER = 150.0f;
        const float OK_LOWER = -150.0f;
        const float GREAT_UPPER = 80.0f;
        const float GREAT_LOWER = -80.0f;
        const float EXCELLENT_UPPER = 30.0f;
        const float EXCELLENT_LOWER = -30.0f;
    }
}

// Button
namespace Menu {
    namespace Button {
        // Main menu
        const int PLAY_X = 390;
        const int PLAY_Y = 100;
        const int PLAY_WIDTH = 500;
        const int PLAY_HEIGHT = 200;

        const int QUIT_X = 390;
        const int QUIT_Y = 400;
        const int QUIT_WIDTH = 500;
        const int QUIT_HEIGHT = 200;

        // Levels button
        const int LEVEL_WIDTH = 150;
        const int LEVEL_HEIGHT = 150;
        const int LEVEL_SPACING = 50;
        const int LEVELS_START_X = (Screen::WIDTH - (2 * LEVEL_WIDTH + LEVEL_SPACING)) / 2;
        const int LEVELS_START_Y = (Screen::HEIGHT - (2 * LEVEL_HEIGHT + LEVEL_SPACING)) / 2;
        const int LEVELS_ROW2_Y = LEVELS_START_Y + LEVEL_HEIGHT + LEVEL_SPACING;

        const int RETURN_X = 15;
        const int RETURN_Y = 15;
        const int RETURN_SIZE = 100;


        const int LEVELS_TEXT_WIDTH = 200;
        const int LEVELS_TEXT_HEIGHT = 50;        
        const int LEVELS_TEXT_X = (Screen::WIDTH - LEVELS_TEXT_WIDTH - 325) / 2;
        const int LEVELS_TEXT_Y = 50;
    }
}

// Game play screen
namespace GameUI {
    namespace Lane {
        const int Y = (Screen::HEIGHT - 175) / 2;
        const int HEIGHT = 235;
    }

    namespace Drum {
        const int X = 15;
        const int Y = (Screen::HEIGHT - 125) / 2;
        const int SIZE = 150;
    }

    namespace Hitbox {
        const int X = 200;
        const int Y = (Screen::HEIGHT - 150) / 2;
        const int SIZE = 150;
    }

    namespace Score {
        const int X = 10;
        const int Y = 10;
        const int WIDTH = 300;
        const int HEIGHT = 50;
    }

    namespace Streak {
        const int X = 10;
        const int Y = 70;
        const int WIDTH = 50;
        const int HEIGHT = 50;
    }

    namespace Accuracy {
        const int X = 10;
        const int Y = 130;
        const int WIDTH = 50;
        const int HEIGHT = 50;
    }

    namespace Time {
        const int X = 10;
        const int Y = 190;
        const int WIDTH = 50;
        const int HEIGHT = 50;
    }

    namespace NoteScore {
        const int X = 235;
        const int Y = (Screen::HEIGHT + 180) / 2;
        const int WIDTH = 150;
        const int HEIGHT = 50;
    }

    namespace Health {
        const int X = Screen::WIDTH - 300;
        const int Y = 10;
        const int WIDTH = 150;
        const int HEIGHT = 50;
    }
    namespace StreakFlame{
        const int X = -175;
        const int Y = Screen::HEIGHT - 165;
        const int WIDTH = Screen::WIDTH + 350;
        const int HEIGHT = 250;
        const int APPEAR = 15;
    }
    const int TEXT_SIZE = 34;

    namespace PauseBoard {
        const int WIDTH = 800;
        const int HEIGHT = 600;
        const int X = (Screen::WIDTH - WIDTH) / 2;
        const int Y = (Screen::HEIGHT - HEIGHT) / 2;
    }

    namespace PauseButton {
        const int WIDTH = 400;
        const int HEIGHT = 100;
        const int SPACING = 40;
        const int START_X = PauseBoard::X + (PauseBoard::WIDTH - WIDTH) / 2;
        const int START_Y = PauseBoard::Y + (PauseBoard::HEIGHT - (3 * HEIGHT + 2 * SPACING)) / 2;
    }
}

// End game screen
namespace EndGame {
    const float SCOREBOARD_SCALE = 0.9f;
    const int SCOREBOARD_X = (Screen::WIDTH - (Screen::WIDTH * SCOREBOARD_SCALE)) / 2;
    const int SCOREBOARD_Y = (Screen::HEIGHT - (Screen::HEIGHT * SCOREBOARD_SCALE)) / 2;
    const int SCOREBOARD_WIDTH = Screen::WIDTH * SCOREBOARD_SCALE;
    const int SCOREBOARD_HEIGHT = Screen::HEIGHT * SCOREBOARD_SCALE;

    const int TEXT_OFFSET_X = 200;
    const int TEXT_OFFSET_Y = 175;
    const int TEXT_SPACING = 40;
    const int TEXT_WIDTH = 500;
    const int TEXT_HEIGHT = 40;
}
