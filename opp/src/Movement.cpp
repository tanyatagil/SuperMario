#include "Movement.hpp"
#include "QuestionBrick.hpp"
#include "Brick.hpp"
#include "GameObject.hpp"
#include "GameController.hpp"

bool Movement::applyGravity(GameObject& object, GameController& level) {
    object.isFlying = true;
    object.vertSpeed += 0.05f;
    object.y += object.vertSpeed;

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

        if (brick->cType == '+') {
            level.goNextLevel();
            return false;
        }

        break;
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
    level.mario.x -= dx;

    for (auto& brick : level.bricks) {
        if (level.mario.collidesWith(*brick)) {
            level.mario.x += dx;
            return;
        }
    }

    level.mario.x += dx;

    for (auto& brick : level.bricks) {
        brick->x += dx;
    }

    for (auto& entity : level.entities) {
        entity->x += dx;
    }
}
