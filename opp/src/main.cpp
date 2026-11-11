#include "GameController.hpp"
#include "Map.hpp"

#include <windows.h>

int main() {
    GameController game;
    game.load(1);

    Map map;

    while (GetKeyState(VK_ESCAPE) >= 0) {
        map.clear();
        game.update();
        game.draw(map);
        map.render();
        Sleep(10);
    }

    return 0;
}
