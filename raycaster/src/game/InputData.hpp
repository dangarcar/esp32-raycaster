#pragma once

#include <stdint.h>
#include <string.h>

constexpr int UP = 5;
constexpr int DOWN = 4;
constexpr int LEFT = 3;
constexpr int RIGHT = 2;

enum class Direction {
    UP, LEFT, DOWN, RIGHT, UP_LEFT, UP_RIGHT, STOPPED
};

struct InputData {
    int16_t x;
    int16_t y;
    uint16_t flags;
    bool correct;

    Direction getDirection() const;
    bool leftClick() const { return flags & (1 << 7); }
    bool rightClick() const { return flags & (1 << 6); }
};

inline void directionToString(Direction dir, char* out) {
    switch (dir) {
    case Direction::UP: strcpy(out, "UP"); break;
    case Direction::UP_LEFT: strcpy(out, "UP_LEFT"); break;
    case Direction::UP_RIGHT: strcpy(out, "UP_RIGHT"); break;
    case Direction::LEFT: strcpy(out, "LEFT"); break;
    case Direction::RIGHT: strcpy(out, "RIGHT"); break;
    case Direction::DOWN: strcpy(out, "DOWN"); break;
    case Direction::STOPPED: strcpy(out, "STOPPED"); break;
    }
}

inline Direction InputData::getDirection() const {
    Direction dir = Direction::STOPPED;

    bool up = flags & (1 << UP);
    bool left = flags & (1 << LEFT);
    bool right = flags & (1 << RIGHT);

    if(up) {
        if(left) 
            dir = Direction::UP_LEFT;
        else if(right)
            dir = Direction::UP_RIGHT;
        else
            dir = Direction::UP;
    }
    else if(right)
        dir = Direction::RIGHT;
    else if(left)
        dir = Direction::LEFT;
    else if(flags & (1 << DOWN))
        dir = Direction::DOWN;
    
    return dir;
}