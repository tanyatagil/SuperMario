#pragma once

#include "GameObject.hpp"

class GameController;

class Mario : public GameObject {
public:
    Mario();

    void reset(float newX = 39, float newY = 10);
    void handleInput(GameController& level);
    void update(GameController& level) override;
};
