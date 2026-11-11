#include "Mario.hpp"

#include "GameController.hpp"
#include "Movement.hpp"
#include "constants.hpp"

#include <ncurses.h>
Mario::Mario()
    : GameObject(39, 10, 3, 3, '@') {}

void Mario::reset(float newX, float newY) {
    x = newX;
    y = newY;
    vertSpeed = 0;
    horizSpeed = 0;
    isFlying = false;
}

void Mario::handleInput(GameController& level) {
    int key;
    while ((key = getch()) != ERR) {
        if (key == 27) {
            level.quitRequested = true;
            return;
        }
        if (key == ' ' && !isFlying) {
            vertSpeed = -1.0f;
        } else if (key == 'a' || key == 'A' || key == KEY_LEFT) {
            Movement::scrollMap(level, 2.5f);
        } else if (key == 'd' || key == 'D' || key == KEY_RIGHT) {
            Movement::scrollMap(level, -2.5f);
        }
    }
}

void Mario::update(GameController& level) {
    handleInput(level);

    if (y > MAP_H) {
        level.playerDead();
        return;
    }

    Movement::applyGravity(*this, level);
}
