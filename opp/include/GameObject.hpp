#pragma once

#include "Map.hpp"

class GameController;

class GameObject {
public:
    float x;
    float y;
    float width;
    float height;
    float vertSpeed = 0;
    float horizSpeed = 0;
    bool isFlying = false;
    char cType;
    bool markedForDelete = false;

    GameObject(float x, float y, float width, float height, char type);
    virtual ~GameObject() = default;

    bool collidesWith(const GameObject& other) const;
    void drawOn(Map& map) const;
    virtual void update(GameController& level) {}
};