#include "GameController.hpp"
#include "LevelCreator.hpp"

#include <ncurses.h>

void GameController::load(int levelNumber) {
    LevelCreator::load(*this, levelNumber);
}

void GameController::playerDead() {
    napms(500);
    needReload = true;
}

void GameController::goNextLevel() {
    levelNum++;

    if (levelNum > maxLevel) {
        levelNum = 1;
    }

    napms(500);
    load(levelNum);
}

void GameController::purgeEntities() {
    entities.erase(
        std::remove_if(
            entities.begin(),
            entities.end(),
            [](const std::unique_ptr<GameObject>& object) {
                return object->markedForDelete;
            }
        ),
        entities.end()
    );
}

void GameController::update() {
    mario.update(*this);

    if (needReload) {
        load(levelNum);
        return;
    }

    int count = static_cast<int>(entities.size());

    for (int i = 0; i < count; i++) {
        if (!entities[i]->markedForDelete) {
            entities[i]->update(*this);
        }
    }

    purgeEntities();
}

void GameController::draw(Map& map) {
    for (auto& brick : bricks) {
        brick->drawOn(map);
    }

    for (auto& entity : entities) {
        entity->drawOn(map);
    }

    mario.drawOn(map);
    map.putScore(score);
}
