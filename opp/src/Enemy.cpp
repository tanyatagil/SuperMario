#include "Enemy.hpp"

#include "GameController.hpp"
#include "Movement.hpp"
#include "constants.hpp"
Enemy::Enemy(float x, float y)
    : GameObject(x, y, 3, 2, 'o') {
    horizSpeed = 0.2f;
}

void Enemy::update(GameController& level) {
    Movement::applyGravity(*this, level);
    Movement::applyHorizontalMove(*this, level);

    if (y > MAP_H) {
        markedForDelete = true;
        return;
    }

    if (collidesWith(level.mario)) {
        bool stomped = level.mario.isFlying &&
                       level.mario.vertSpeed > 0 &&
                       level.mario.y + level.mario.height < y + height * 0.5f;

        if (stomped) {
            level.score += 50;
            markedForDelete = true;
        } else {
            level.playerDead();
        }
    }
}
