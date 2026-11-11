#include "Map.hpp"

#include <ncurses.h>

#include <algorithm>
#include <cstdio>

void Map::clear() {
    for (auto& row : grid) {
        std::fill(row, row + MAP_W, ' ');
        row[MAP_W] = '\0';
    }
}

void Map::setChar(int x, int y, char ch) {
    if (x >= 0 && x < MAP_W && y >= 0 && y < MAP_H) {
        grid[y][x] = ch;
    }
}

void Map::putScore(int score) {
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "Score: %d", score);

    for (int i = 0; buffer[i] != '\0' && 5 + i < MAP_W; ++i) {
        grid[1][5 + i] = buffer[i];
    }
}

void Map::render() {
    move(0, 0);
    for (const auto& row : grid) {
        addnstr(row, MAP_W);
        addch('\n');
    }
    refresh();
}
