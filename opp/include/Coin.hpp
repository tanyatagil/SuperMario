#pragma once

#include "GameObject.hpp"

class Brick;
class GameController;

class Coin : public GameObject {
public:
    Brick* sourceBrick = nullptr;

    Coin(float x, float y, Brick* source = nullptr);
    void update(GameController& level) override;
};
