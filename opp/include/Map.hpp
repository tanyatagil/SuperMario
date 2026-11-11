#pragma once

#include "constants.hpp"

class Map {
    char grid[MAP_H][MAP_W + 1];

public:
    void clear();
    void setChar(int x, int y, char ch);
    void putScore(int score);
    void render();
};
