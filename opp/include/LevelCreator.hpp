#pragma once

class GameController;

class LevelCreator {
public:
    static void load(GameController& level, int levelNumber);

private:
    static void loadFirstLevel(GameController& level);
    static void loadSecondLevel(GameController& level);
    static void loadThirdLevel(GameController& level);
};
