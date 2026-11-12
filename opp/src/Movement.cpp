#include "Movement.hpp"
#include "QuestionBrick.hpp"
#include "Brick.hpp"
#include "GameObject.hpp"
#include "GameController.hpp"

#include <algorithm>
#include <cmath>

bool Movement::applyGravity(GameObject& object, GameController& level) {
    object.isFlying = true;
    object.vertSpeed += 0.05f;
    object.y += object.vertSpeed;
    bool reachedFinish = false;

    for (auto& brick : level.bricks) {
        if (!object.collidesWith(*brick)) {
            continue;
        }

        if (object.vertSpeed > 0) {
            object.isFlying = false;
            object.y = brick->y - object.height;
        } else {
            object.y = brick->y + brick->height;
        }

        if (brick->cType == '?' && object.vertSpeed < 0 && &object == &level.mario) {
            auto* questionBrick = dynamic_cast<QuestionBrick*>(brick.get());
            if (questionBrick && !questionBrick->used) {
                questionBrick->onHitFromBelow(level);
            }
        }

        object.vertSpeed = 0;

        if (brick->cType == '+' && &object == &level.mario) {
            reachedFinish = true;
        }

        break;
    }

    if (reachedFinish) {
        level.goNextLevel();
        return false;
    }

    return true;
}

void Movement::applyHorizontalMove(GameObject& object, GameController& level) {
    object.x += object.horizSpeed;

    for (auto& brick : level.bricks) {
        if (object.collidesWith(*brick)) {
            object.x -= object.horizSpeed;
            object.horizSpeed = -object.horizSpeed;
            return;
        }
    }

    if (object.cType == 'o') {
        float savedY = object.y;
        float savedVertSpeed = object.vertSpeed;
        bool savedIsFlying = object.isFlying;

        object.y += 0.05f;
        bool onEdge = true;

        for (auto& brick : level.bricks) {
            if (object.collidesWith(*brick)) {
                onEdge = false;
                break;
            }
        }

        object.y = savedY;
        object.vertSpeed = savedVertSpeed;
        object.isFlying = savedIsFlying;

        if (onEdge) {
            object.x -= object.horizSpeed;
            object.horizSpeed = -object.horizSpeed;
        }
    }
}

void Movement::scrollMap(GameController& level, float dx) {
    const float direction = dx < 0 ? -1.0f : 1.0f;
    float remaining = std::abs(dx);

    while (remaining > 0.0f) {
        const float step = direction * std::min(0.5f, remaining);
        level.mario.x -= step;

        bool blocked = false;
        for (auto& brick : level.bricks) {
            const bool standingOnTop =
                level.mario.y + level.mario.height <= brick->y + 0.1f;
            if (level.mario.collidesWith(*brick) && !standingOnTop) {
                blocked = true;
                break;
            }
        }

        if (blocked) {
            level.mario.x += step;
            return;
        }

        level.mario.x += step;

        for (auto& brick : level.bricks) {
            brick->x += step;
        }
        for (auto& entity : level.entities) {
            entity->x += step;
        }

        remaining -= std::abs(step);
    }
}
