#include "Mario.hpp"

#include "GameController.hpp"
#include "Movement.hpp"
#include "constants.hpp"

#include <windows.h>
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
    if (!isFlying && GetKeyState(VK_SPACE) < 0) {
        vertSpeed = -1.0f;
    }

    if (GetKeyState('A') < 0) {
        Movement::scrollMap(level, 1);
    }

    if (GetKeyState('D') < 0) {
        Movement::scrollMap(level, -1);
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
