#include "GameObject.hpp"

#include <cmath>
GameObject::GameObject(float x, float y, float width, float height, char type)
    : x(x), y(y), width(width), height(height), cType(type) {}

bool GameObject::collidesWith(const GameObject& other) const {
    return x + width > other.x &&
           x < other.x + other.width &&
           y + height > other.y &&
           y < other.y + other.height;
}

void GameObject::drawOn(Map& map) const {
    int ix = static_cast<int>(std::round(x));
    int iy = static_cast<int>(std::round(y));
    int iw = static_cast<int>(std::round(width));
    int ih = static_cast<int>(std::round(height));

    for (int i = ix; i < ix + iw; i++) {
        for (int j = iy; j < iy + ih; j++) {
            map.setChar(i, j, cType);
        }
    }
}