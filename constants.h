#pragma once
#include <SDL2/SDL.h>

class Screen {
public:
    static const int WIDTH;
    static const int HEIGHT;
    static const char* TITLE;
};

class GameTiming {
public:
    static const int FPS;
    static const int FRAME_DELAY;
};

class Game {
public:
    class Note {
    public:
        // Timing thresholds
        static const float EXCELLENT_THRESHOLD;
        static const float GREAT_THRESHOLD;
        static const float OK_THRESHOLD;
        static const float MISS_THRESHOLD;
        static const float LATE_THRESHOLD;

        // Range boundaries
        static const float EXCELLENT_RANGE_START;
        static const float EXCELLENT_RANGE_END;
        static const float GREAT_RANGE_START;
        static const float GREAT_RANGE_END;
        static const float OK_RANGE_START;
        static const float OK_RANGE_END;

        // Gap calculation thresholds
        static const float MISS_UPPER;
        static const float MISS_LOWER;
        static const float OK_UPPER;
        static const float OK_LOWER;
        static const float GREAT_UPPER;
        static const float GREAT_LOWER;
        static const float EXCELLENT_UPPER;
        static const float EXCELLENT_LOWER;

        // Score values
        static const int OK_SCORE;
        static const int GREAT_SCORE;
        static const int EXCELLENT_SCORE;

        // Note dimensions and positions
        static const int WIDTH;
        static const int HEIGHT;
        static const int SPAWN_X;
        static const int Y;
        static const int HIT_X;
    };
};

class Menu {
public:
    class Button {
    public:
        // Play button
        static const int PLAY_X;
        static const int PLAY_Y;
        static const int PLAY_WIDTH;
        static const int PLAY_HEIGHT;

        // Quit button
        static const int QUIT_X;
        static const int QUIT_Y;
        static const int QUIT_WIDTH;
        static const int QUIT_HEIGHT;

        // Levels text
        static const int LEVELS_TEXT_X;
        static const int LEVELS_TEXT_Y;
        static const int LEVELS_TEXT_WIDTH;
        static const int LEVELS_TEXT_HEIGHT;

        // Return button
        static const int RETURN_X;
        static const int RETURN_Y;
        static const int RETURN_SIZE;

        // Level buttons
        static const int LEVEL_WIDTH;
        static const int LEVEL_HEIGHT;
        static const int LEVEL_SPACING;
        static const int LEVELS_START_X;
        static const int LEVELS_START_Y;
        static const int LEVELS_ROW2_Y;
    };
};

class GameUI {
public:
    class Lane {
    public:
        static const int Y;
        static const int HEIGHT;
    };

    class Drum {
    public:
        static const int X;
        static const int Y;
        static const int SIZE;
    };

    class Hitbox {
    public:
        static const int X;
        static const int Y;
        static const int SIZE;
    };

    class Score {
    public:
        static const int X;
        static const int Y;
        static const int WIDTH;
        static const int HEIGHT;
    };

    class Streak {
    public:
        static const int X;
        static const int Y;
        static const int WIDTH;
        static const int HEIGHT;
    };

    class NoteScore {
    public:
        static const int X;
        static const int Y;
        static const int WIDTH;
        static const int HEIGHT;
    };

    class Health {
    public:
        static const int X;
        static const int Y;
        static const int WIDTH;
        static const int HEIGHT;
    };
};

class EndGame {
public:
    static const float SCOREBOARD_SCALE;
    static const int SCOREBOARD_X;
    static const int SCOREBOARD_Y;
    static const int SCOREBOARD_WIDTH;
    static const int SCOREBOARD_HEIGHT;

    static const int TEXT_OFFSET_X;
    static const int TEXT_OFFSET_Y;
    static const int TEXT_SPACING;
    static const int TEXT_WIDTH;
    static const int TEXT_HEIGHT;
};

class GameSettings {
public:
    static const int DEFAULT_HEALTH;
    static const int LEVEL2_SPEED;
    static const int LEVEL3_SPEED;
    static const int LEVEL4_SPEED;
};
 