#pragma once

#include "Brick.hpp"

class GameController;

class QuestionBrick : public Brick {
public:
    bool used = false;

    QuestionBrick(float x, float y, float width, float height);
    void onHitFromBelow(GameController& level);
};
