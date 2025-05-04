#pragma once

// Window
namespace Screen {
    extern const int WIDTH;
    extern const int HEIGHT;
    extern const char* TITLE;
}

// Game timing
namespace GameTiming {
    extern const int FPS;
    extern const int FRAME_DELAY;
}

// Score values
namespace Game {
    namespace Note {
        extern const int OK_SCORE;
        extern const int GREAT_SCORE;
        extern const int EXCELLENT_SCORE;

        // Note
        extern const int WIDTH;
        extern const int HEIGHT;
        extern const int SPAWN_X;
        extern const int Y;
        extern const int HIT_X;
        extern const int DEATH_X;
        // Game note
        extern const float MISS_UPPER;
        extern const float MISS_LOWER;
        extern const float OK_UPPER;
        extern const float OK_LOWER;
        extern const float GREAT_UPPER;
        extern const float GREAT_LOWER;
        extern const float EXCELLENT_UPPER;
        extern const float EXCELLENT_LOWER;
    }
}

// Button
namespace Menu {
    namespace Button {
        // Main menu
        extern const int PLAY_X;
        extern const int PLAY_Y;
        extern const int PLAY_WIDTH;
        extern const int PLAY_HEIGHT;

        extern const int QUIT_X;
        extern const int QUIT_Y;
        extern const int QUIT_WIDTH;
        extern const int QUIT_HEIGHT;

        // Levels button
        extern const int LEVEL_WIDTH;
        extern const int LEVEL_HEIGHT;
        extern const int LEVEL_SPACING;
        extern const int LEVELS_START_X;
        extern const int LEVELS_START_Y;
        extern const int LEVELS_ROW2_Y;

        extern const int RETURN_X;
        extern const int RETURN_Y;
        extern const int RETURN_SIZE;


        extern const int LEVELS_TEXT_WIDTH;
        extern const int LEVELS_TEXT_HEIGHT;        
        extern const int LEVELS_TEXT_X;
        extern const int LEVELS_TEXT_Y;
    }
}

// Game play screen
namespace GameUI {
    namespace Lane {
        extern const int Y;
        extern const int HEIGHT;
    }

    namespace Drum {
        extern const int X;
        extern const int Y;
        extern const int SIZE;
    }

    namespace Hitbox {
        extern const int X;
        extern const int Y;
        extern const int SIZE;
    }

    namespace Score {
        extern const int X;
        extern const int Y;
        extern const int WIDTH;
        extern const int HEIGHT;
    }

    namespace Streak {
        extern const int X;
        extern const int Y;
        extern const int WIDTH;
        extern const int HEIGHT;
    }

    namespace Accuracy {
        extern const int X;
        extern const int Y;
        extern const int WIDTH;
        extern const int HEIGHT;
    }

    namespace Time {
        extern const int X;
        extern const int Y;
        extern const int WIDTH;
        extern const int HEIGHT;
    }

    namespace NoteScore {
        extern const int X;
        extern const int Y;
        extern const int WIDTH;
        extern const int HEIGHT;
    }

    namespace Health {
        extern const int X;
        extern const int Y;
        extern const int WIDTH;
        extern const int HEIGHT;
    }
    namespace StreakFlame{
        extern const int X;
        extern const int Y;
        extern const int WIDTH;
        extern const int HEIGHT;
        extern const int APPEAR;
    }
    extern const int TEXT_SIZE;

    namespace PauseBoard {
        extern const int WIDTH;
        extern const int HEIGHT;
        extern const int X;
        extern const int Y;
    }

    namespace PauseButton {
        extern const int WIDTH;
        extern const int HEIGHT;
        extern const int SPACING;
        extern const int START_X;
        extern const int START_Y;
    }
}

// End game screen
namespace EndGame {
    extern const float SCOREBOARD_SCALE;
    extern const int SCOREBOARD_X;
    extern const int SCOREBOARD_Y;
    extern const int SCOREBOARD_WIDTH;
    extern const int SCOREBOARD_HEIGHT;

    extern const int TEXT_OFFSET_X;
    extern const int TEXT_OFFSET_Y;
    extern const int TEXT_SPACING;
    extern const int TEXT_WIDTH;
    extern const int TEXT_HEIGHT;
}