#include "Coin.hpp"

#include "Brick.hpp"
#include "GameController.hpp"
#include "Movement.hpp"
#include "constants.hpp"
Coin::Coin(float x, float y, Brick* source)
    : GameObject(x, y, 3, 2, '$'), sourceBrick(source) {
    vertSpeed = -0.7f;
    horizSpeed = 0.2f;
}

void Coin::update(GameController& level) {
    if (vertSpeed < 0) {
        vertSpeed += 0.05f;
        y += vertSpeed;
    } else {
        sourceBrick = nullptr;
        Movement::applyGravity(*this, level);
        Movement::applyHorizontalMove(*this, level);
    }

    if (collidesWith(level.mario)) {
        level.score += 100;
        markedForDelete = true;
        return;
    }

    if (y > MAP_H) {
        markedForDelete = true;
    }
}
