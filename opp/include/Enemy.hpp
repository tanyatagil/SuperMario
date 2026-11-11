#pragma once

#include "GameObject.hpp"

class GameController;

class Enemy : public GameObject {
public:
    Enemy(float x, float y);
    void update(GameController& level) override;
};
