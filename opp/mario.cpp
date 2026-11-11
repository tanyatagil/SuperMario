#include <ncurses.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace {
constexpr int mapWidth = 80;
constexpr int mapHeight = 25;

struct Object {
    float x;
    float y;
    float width;
    float height;
    float verticalSpeed = 0.0f;
    float horizontalSpeed = 0.0f;
    char symbol;
    bool flying = false;
};

using Objects = std::vector<Object>;

bool collides(const Object& first, const Object& second) {
    return first.x < second.x + second.width &&
           first.x + first.width > second.x &&
           first.y < second.y + second.height &&
           first.y + first.height > second.y;
}

Object makeObject(float x, float y, float width, float height, char symbol) {
    return {x, y, width, height, 0.0f, 0.0f, symbol, false};
}

void clearMap(char map[mapHeight][mapWidth + 1]) {
    for (int y = 0; y < mapHeight; ++y) {
        std::fill_n(map[y], mapWidth, ' ');
        map[y][mapWidth] = '\0';
    }
}

void drawObject(char map[mapHeight][mapWidth + 1], const Object& object) {
    const int left = static_cast<int>(std::round(object.x));
    const int top = static_cast<int>(std::round(object.y));
    const int width = static_cast<int>(std::round(object.width));
    const int height = static_cast<int>(std::round(object.height));

    for (int y = top; y < top + height; ++y) {
        for (int x = left; x < left + width; ++x) {
            if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
                map[y][x] = object.symbol;
            }
        }
    }
}

void createLevel(int level, Objects& bricks, Objects& entities, Object& mario) {
    bricks.clear();
    entities.clear();
    mario = makeObject(10, 10, 3, 3, '@');

    bricks.push_back(makeObject(0, 21, 60, 4, '#'));
    bricks.push_back(makeObject(66, 21, 35, 4, '#'));
    bricks.push_back(makeObject(25, 14, 8, 2, '?'));
    bricks.push_back(makeObject(45, 10, 8, 2, '?'));
    bricks.push_back(makeObject(92, 16, 8, 2, '+'));

    const int enemyCount = level + 1;
    for (int i = 0; i < enemyCount; ++i) {
        Object enemy = makeObject(35.0f + i * 18.0f, 18, 3, 2, 'o');
        enemy.horizontalSpeed = i % 2 == 0 ? 0.12f : -0.12f;
        entities.push_back(enemy);
    }
}

void scrollMap(float distance, Object& mario, Objects& bricks, Objects& entities) {
    mario.x -= distance;
    for (const Object& brick : bricks) {
        if (collides(mario, brick)) {
            mario.x += distance;
            return;
        }
    }

    mario.x += distance;
    for (Object& brick : bricks) {
        brick.x += distance;
    }
    for (Object& entity : entities) {
        entity.x += distance;
    }
}

bool applyGravity(Object& object, Objects& bricks, Object* mario) {
    object.flying = true;
    object.verticalSpeed += 0.05f;
    object.y += object.verticalSpeed;

    for (Object& brick : bricks) {
        if (!collides(object, brick)) {
            continue;
        }

        if (object.verticalSpeed > 0) {
            object.y = brick.y - object.height;
            object.flying = false;
        } else {
            object.y = brick.y + brick.height;
            if (&object == mario && brick.symbol == '?') {
                // A question brick becomes an ordinary brick after a hit.
                brick.symbol = '-';
            }
        }
        object.verticalSpeed = 0;
        return brick.symbol != '+';
    }
    return true;
}

void updateEnemies(Objects& entities, Objects& bricks) {
    for (Object& entity : entities) {
        entity.x += entity.horizontalSpeed;
        for (const Object& brick : bricks) {
            if (collides(entity, brick)) {
                entity.x -= entity.horizontalSpeed;
                entity.horizontalSpeed = -entity.horizontalSpeed;
                break;
            }
        }
        applyGravity(entity, bricks, nullptr);
    }
}

void render(char map[mapHeight][mapWidth + 1], int score, int level,
            const Object& mario, const Objects& bricks, const Objects& entities) {
    for (const Object& brick : bricks) {
        drawObject(map, brick);
    }
    for (const Object& entity : entities) {
        drawObject(map, entity);
    }
    drawObject(map, mario);

    const std::string status = "Score: " + std::to_string(score) +
                               "  Level: " + std::to_string(level);
    for (std::size_t i = 0; i < status.size() && i + 2 < mapWidth; ++i) {
        map[1][i + 2] = status[i];
    }

    move(0, 0);
    for (int y = 0; y < mapHeight; ++y) {
        printw("%s\n", map[y]);
    }
    refresh();
}
}  // namespace

int main() {
    char map[mapHeight][mapWidth + 1];
    Object mario = makeObject(0, 0, 3, 3, '@');
    Objects bricks;
    Objects entities;
    int level = 1;
    int score = 0;

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, true);
    nodelay(stdscr, true);
    curs_set(0);
    createLevel(level, bricks, entities, mario);

    bool running = true;
    while (running) {
        int key;
        while ((key = getch()) != ERR) {
            if (key == 27) {
                running = false;
            } else if (key == ' ' && !mario.flying) {
                mario.verticalSpeed = -1.0f;
            } else if (key == 'a' || key == 'A' || key == KEY_LEFT) {
                scrollMap(1.5f, mario, bricks, entities);
            } else if (key == 'd' || key == 'D' || key == KEY_RIGHT) {
                scrollMap(-1.5f, mario, bricks, entities);
            }
        }

        if (!running) {
            break;
        }
        if (mario.y > mapHeight ||
            !applyGravity(mario, bricks, &mario)) {
            score = 0;
            createLevel(level, bricks, entities, mario);
        }
        updateEnemies(entities, bricks);

        for (auto entity = entities.begin(); entity != entities.end();) {
            if (collides(mario, *entity)) {
                if (mario.flying && mario.verticalSpeed > 0) {
                    score += 50;
                    entity = entities.erase(entity);
                } else {
                    score = 0;
                    createLevel(level, bricks, entities, mario);
                    break;
                }
            } else {
                ++entity;
            }
        }

        clearMap(map);
        render(map, score, level, mario, bricks, entities);
        napms(10);
    }

    endwin();
    return 0;
}
