#pragma once

class GameObject;
class GameController;

class Movement {
public:
    static bool applyGravity(GameObject& object, GameController& level);
    static void applyHorizontalMove(GameObject& object, GameController& level);
    static void scrollMap(GameController& level, float dx);
};
