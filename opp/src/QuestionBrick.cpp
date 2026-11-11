#include "QuestionBrick.hpp"

#include "Coin.hpp"
#include "GameController.hpp"
QuestionBrick::QuestionBrick(float x, float y, float width, float height)
    : Brick(x, y, width, height, '?') {}

void QuestionBrick::onHitFromBelow(GameController& level) {
    used = true;
    cType = '-';
    level.addEntity<Coin>(x, y - 3, this);
}
