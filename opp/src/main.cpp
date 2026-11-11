#include "GameController.hpp"
#include "Map.hpp"

#include <ncurses.h>

int main() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    GameController game;
    game.load(1);

    Map map;

    while (!game.quitRequested) {
        map.clear();
        game.update();
        game.draw(map);
        map.render();
        napms(10);
    }

    endwin();
    return 0;
}
