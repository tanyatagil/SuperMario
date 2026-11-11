#pragma once

#include "GameObject.hpp"

class Brick : public GameObject {
public:
    Brick(float x, float y, float width, float height, char type);
};
