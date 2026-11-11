#pragma warning(disable : 4996)

#include "Map.hpp"

#include <cstdio>
#include <cstring>
#include <windows.h>

static void setCur(int x, int y) {
    COORD coord{static_cast<SHORT>(x), static_cast<SHORT>(y)};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Map::clear() {
    for (int i = 0; i < MAP_W; i++) {
        grid[0][i] = ' ';
    }

    grid[0][MAP_W] = '\0';

    for (int j = 1; j < MAP_H; j++) {
        sprintf(grid[j], grid[0]);
    }
}

void Map::setChar(int x, int y, char ch) {
    if (x >= 0 && x < MAP_W && y >= 0 && y < MAP_H) {
        grid[y][x] = ch;
    }
}

void Map::putScore(int score) {
    char buffer[32];
    sprintf(buffer, "Score: %d", score);
    int length = static_cast<int>(strlen(buffer));

    for (int i = 0; i < length; i++) {
        grid[1][5 + i] = buffer[i];
    }
}

void Map::render() {
    grid[MAP_H - 1][MAP_W - 1] = '\0';
    setCur(0, 0);

    for (int j = 0; j < MAP_H; j++) {
        printf("%s\n", grid[j]);
    }
}
