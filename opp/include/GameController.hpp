#pragma once

#include "Brick.hpp"
#include "Mario.hpp"
#include "Map.hpp"

#include <algorithm>
#include <memory>
#include <vector>

class GameController {
public:
    std::vector<std::unique_ptr<Brick>> bricks;
    std::vector<std::unique_ptr<GameObject>> entities;
    Mario mario;
    int levelNum = 1;
    int score = 0;
    int maxLevel = 3;
    bool needReload = false;
    bool nextLevel = false;

    template<typename T, typename... Args>
    T* addBrick(Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = ptr.get();
        bricks.push_back(std::move(ptr));
        return raw;
    }

    template<typename T, typename... Args>
    T* addEntity(Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = ptr.get();
        entities.push_back(std::move(ptr));
        return raw;
    }

    void load(int levelNumber);
    void playerDead();
    void goNextLevel();
    void purgeEntities();
    void update();
    void draw(Map& map);
};
