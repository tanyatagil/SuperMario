#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>

#define mapHeight 25
#define mapWidth 80

typedef struct SObject {
    float x, y;
    float width, height;
    float vertSpeed;
    bool isFly;
    char cType;
    float horizSpeed;
} TObject;

void clearMap(char map[mapHeight][mapWidth + 1]);
void createLevel(int lvl, int* score, int* level, int maxLvl,
    TObject** brick, int* brickLength,
    TObject** moving, int* movingLength,
    TObject* mario);
void deleteMoving(int i, TObject** moving, int* movingLength);
TObject* getNewBrick(TObject** brick, int* brickLength);
TObject* getNewMoving(TObject** moving, int* movingLength);
void horizonMoveMap(float dx, TObject* brick, int brickLength, TObject* moving, int movingLength, TObject* mario);
void horizonMoveObject(TObject* obj, int* level, int* score, int maxLvl,
    TObject** brick, int* brickLength,
    TObject** moving, int* movingLength,
    TObject* mario,
    char map[mapHeight][mapWidth + 1]);
void initObject(TObject* obj, float xPos, float yPos, float objectWidth, float objectHeight, char inType);
bool isCollision(TObject obj1, TObject obj2);
bool isPosInMap(int x, int y);
void marioCollision(int* score, int* level, int maxLvl,
    TObject** brick, int* brickLength,
    TObject** moving, int* movingLength,
    TObject* mario,
    char map[mapHeight][mapWidth + 1]);
void playerDead(int* level, int* score, int maxLvl,
    TObject** brick, int* brickLength,
    TObject** moving, int* movingLength,
    TObject* mario,
    char map[mapHeight][mapWidth + 1]);
void putObjectOnMap(TObject obj, char map[mapHeight][mapWidth + 1]);
void putScoreOnMap(int score, char map[mapHeight][mapWidth + 1]);
void setCur(int x, int y);
void setObjectPos(TObject* obj, float xPos, float yPos);
void showMap(char map[mapHeight][mapWidth + 1]);
void vertMoveObject(TObject* obj, int* level, int* score, int maxLvl,
    TObject** brick, int* brickLength,
    TObject** moving, int* movingLength,
    TObject* mario,
    char map[mapHeight][mapWidth + 1]);

int main() {
    int level = 1;
    int score = 0;
    int maxLvl = 3;

    char map[mapHeight][mapWidth + 1];
    TObject mario;
    TObject* brick = NULL;
    int brickLength = 0;

    TObject* moving = NULL;
    int movingLength = 0;

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, true);
    nodelay(stdscr, true);
    curs_set(0);

    createLevel(level, &score, &level, maxLvl, &brick, &brickLength, &moving, &movingLength, &mario);

    bool running = true;
    while (running)
    {
        clearMap(map);

        int key;
        while ((key = getch()) != ERR)
        {
            if (key == 27)
            {
                running = false;
                break;
            }

            if (key == ' ' && mario.isFly == false)
                mario.vertSpeed = -1;

            if (key == 'a' || key == 'A' || key == KEY_LEFT)
                horizonMoveMap(2.0f, brick, brickLength, moving, movingLength, &mario);

            if (key == 'd' || key == 'D' || key == KEY_RIGHT)
                horizonMoveMap(-2.0f, brick, brickLength, moving, movingLength, &mario);
        }

        if (!running)
            break;

        if (mario.y > mapHeight)
            playerDead(&level, &score, maxLvl, &brick, &brickLength, &moving, &movingLength, &mario, map);

        vertMoveObject(&mario, &level, &score, maxLvl, &brick, &brickLength, &moving, &movingLength, &mario, map);
        marioCollision(&score, &level, maxLvl, &brick, &brickLength, &moving, &movingLength, &mario, map);

        for (int i = 0; i < brickLength; i++)
            putObjectOnMap(brick[i], map);

        for (int i = 0; i < movingLength; i++)
        {
            vertMoveObject(&moving[i], &level, &score, maxLvl, &brick, &brickLength, &moving, &movingLength, &mario, map);
            horizonMoveObject(&moving[i], &level, &score, maxLvl, &brick, &brickLength, &moving, &movingLength, &mario, map);

            if (moving[i].y > mapHeight)
            {
                deleteMoving(i, &moving, &movingLength);
                i--;
                continue;
            }

            putObjectOnMap(moving[i], map);
        }

        putObjectOnMap(mario, map);
        putScoreOnMap(score, map);

        setCur(0, 0);
        showMap(map);

        usleep(10000);
    }

    free(brick);
    free(moving);

    endwin();
    return 0;
}

void clearMap(char map[mapHeight][mapWidth + 1]) {
    for (int j = 0; j < mapHeight; j++)
    {
        for (int i = 0; i < mapWidth; i++)
            map[j][i] = ' ';
        map[j][mapWidth] = '\0';
    }
}

void createLevel(int lvl, int* score, int* level, int maxLvl,
    TObject** brick, int* brickLength,
    TObject** moving, int* movingLength,
    TObject* mario) {

    (void)maxLvl;

    *score = *score;
    *level = *level;

    *brickLength = 0;
    *brick = (TObject*)realloc(*brick, 0);

    *movingLength = 0;
    *moving = (TObject*)realloc(*moving, 0);

    initObject(mario, 39, 10, 3, 3, '@');

    if (lvl == 1)
    {
        initObject(getNewBrick(brick, brickLength), 20, 20, 40, 5, '#');
        initObject(getNewBrick(brick, brickLength), 30, 10, 5, 3, '?');
        initObject(getNewBrick(brick, brickLength), 50, 10, 5, 3, '?');
        initObject(getNewBrick(brick, brickLength), 60, 15, 40, 10, '#');
        initObject(getNewBrick(brick, brickLength), 60, 5, 10, 3, '-');
        initObject(getNewBrick(brick, brickLength), 70, 5, 5, 3, '?');
        initObject(getNewBrick(brick, brickLength), 75, 5, 5, 3, '-');
        initObject(getNewBrick(brick, brickLength), 80, 5, 5, 3, '?');
        initObject(getNewBrick(brick, brickLength), 85, 5, 10, 3, '-');
        initObject(getNewBrick(brick, brickLength), 100, 20, 20, 5, '#');
        initObject(getNewBrick(brick, brickLength), 120, 15, 10, 10, '#');
        initObject(getNewBrick(brick, brickLength), 150, 20, 40, 5, '#');
        initObject(getNewBrick(brick, brickLength), 210, 15, 10, 10, '+');

        initObject(getNewMoving(moving, movingLength), 25, 10, 3, 2, 'o');
        initObject(getNewMoving(moving, movingLength), 80, 10, 3, 2, 'o');
    }

    if (lvl == 2)
    {
        initObject(getNewBrick(brick, brickLength), 20, 20, 40, 5, '#');
        initObject(getNewBrick(brick, brickLength), 60, 15, 10, 10, '#');
        initObject(getNewBrick(brick, brickLength), 80, 20, 20, 5, '#');
        initObject(getNewBrick(brick, brickLength), 120, 15, 10, 10, '#');
        initObject(getNewBrick(brick, brickLength), 150, 20, 40, 5, '#');
        initObject(getNewBrick(brick, brickLength), 210, 15, 10, 10, '+');

        initObject(getNewMoving(moving, movingLength), 25, 10, 3, 2, 'o');
        initObject(getNewMoving(moving, movingLength), 80, 10, 3, 2, 'o');
        initObject(getNewMoving(moving, movingLength), 65, 10, 3, 2, 'o');
        initObject(getNewMoving(moving, movingLength), 120, 10, 3, 2, 'o');
        initObject(getNewMoving(moving, movingLength), 160, 10, 3, 2, 'o');
        initObject(getNewMoving(moving, movingLength), 175, 10, 3, 2, 'o');
    }

    if (lvl == 3)
    {
        initObject(getNewBrick(brick, brickLength), 20, 20, 40, 5, '#');
        initObject(getNewBrick(brick, brickLength), 80, 20, 15, 5, '#');
        initObject(getNewBrick(brick, brickLength), 120, 15, 15, 10, '#');
        initObject(getNewBrick(brick, brickLength), 160, 10, 15, 15, '+');

        initObject(getNewMoving(moving, movingLength), 25, 10, 3, 2, 'o');
        initObject(getNewMoving(moving, movingLength), 50, 10, 3, 2, 'o');
        initObject(getNewMoving(moving, movingLength), 80, 10, 3, 2, 'o');
        initObject(getNewMoving(moving, movingLength), 90, 10, 3, 2, 'o');
        initObject(getNewMoving(moving, movingLength), 120, 10, 3, 2, 'o');
        initObject(getNewMoving(moving, movingLength), 130, 10, 3, 2, 'o');
    }
}

void deleteMoving(int i, TObject** moving, int* movingLength)
{
    (*movingLength)--;
    (*moving)[i] = (*moving)[*movingLength];
    *moving = (TObject*)realloc(*moving, sizeof(**moving) * (*movingLength));
}

TObject* getNewBrick(TObject** brick, int* brickLength)
{
    (*brickLength)++;
    *brick = (TObject*)realloc(*brick, sizeof(**brick) * (*brickLength));
    return *brick + (*brickLength) - 1;
}

TObject* getNewMoving(TObject** moving, int* movingLength)
{
    (*movingLength)++;
    *moving = (TObject*)realloc(*moving, sizeof(**moving) * (*movingLength));
    return *moving + (*movingLength) - 1;
}

void horizonMoveMap(float dx, TObject* brick, int brickLength, TObject* moving, int movingLength, TObject* mario) {
    mario->x -= dx;

    for (int i = 0; i < brickLength; i++)
        if (isCollision(*mario, brick[i])) {
            mario->x += dx;
            return;
        }

    mario->x += dx;

    for (int i = 0; i < brickLength; i++)
        brick[i].x += dx;

    for (int i = 0; i < movingLength; i++)
        moving[i].x += dx;
}

void horizonMoveObject(TObject* obj, int* level, int* score, int maxLvl,
    TObject** brick, int* brickLength,
    TObject** moving, int* movingLength,
    TObject* mario,
    char map[mapHeight][mapWidth + 1])
{
    obj[0].x += obj[0].horizSpeed;

    for (int i = 0; i < *brickLength; i++)
        if (isCollision(obj[0], (*brick)[i]))
        {
            obj[0].x -= obj[0].horizSpeed;
            obj[0].horizSpeed = -obj[0].horizSpeed;
            return;
        }

    if (obj[0].cType == 'o')
    {
        TObject tmp = obj[0];
        vertMoveObject(&tmp, level, score, maxLvl, brick, brickLength, moving, movingLength, mario, map);

        if (tmp.isFly == true)
        {
            obj[0].x -= obj[0].horizSpeed;
            obj[0].horizSpeed = -obj[0].horizSpeed;
        }
    }
}

void initObject(TObject* obj, float xPos, float yPos, float objectWidth, float objectHeight, char inType) {
    setObjectPos(obj, xPos, yPos);
    (*obj).width = objectWidth;
    (*obj).height = objectHeight;
    (*obj).vertSpeed = 0;
    (*obj).isFly = false;
    (*obj).cType = inType;
    (*obj).horizSpeed = 0.2;
}

bool isCollision(TObject obj1, TObject obj2) {
    return ((obj1.x + obj1.width) > obj2.x) && (obj1.x < (obj2.x + obj2.width)) &&
        ((obj1.y + obj1.height) > obj2.y) && (obj1.y < (obj2.y + obj2.height));
}

bool isPosInMap(int x, int y) {
    return ((x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight));
}

void marioCollision(int* score, int* level, int maxLvl,
    TObject** brick, int* brickLength,
    TObject** moving, int* movingLength,
    TObject* mario,
    char map[mapHeight][mapWidth + 1])
{
    for (int i = 0; i < *movingLength; i++)
        if (isCollision(*mario, (*moving)[i]))
        {
            if ((*moving)[i].cType == 'o')
            {
                if ((mario->isFly == true) &&
                    (mario->vertSpeed > 0) &&
                    (mario->y + mario->height < (*moving)[i].y + (*moving)[i].height * 0.5))
                {
                    (*score) += 50;
                    deleteMoving(i, moving, movingLength);
                    i--;
                    continue;
                }

                playerDead(level, score, maxLvl, brick, brickLength, moving, movingLength, mario, map);
                return;
            }

            if ((*moving)[i].cType == '$')
            {
                (*score) += 100;
                deleteMoving(i, moving, movingLength);
                i--;
                continue;
            }
        }
}

void playerDead(int* level, int* score, int maxLvl,
    TObject** brick, int* brickLength,
    TObject** moving, int* movingLength,
    TObject* mario,
    char map[mapHeight][mapWidth + 1])
{
    (void)map;

    napms(500);
    *score = 0;
    createLevel(*level, score, level, maxLvl, brick, brickLength, moving, movingLength, mario);
}

void putObjectOnMap(TObject obj, char map[mapHeight][mapWidth + 1]) {
    int ix = (int)round(obj.x);
    int iy = (int)round(obj.y);
    int objectWidth = (int)round(obj.width);
    int objectHeight = (int)round(obj.height);

    for (int i = ix; i < ix + objectWidth; i++)
        for (int j = iy; j < iy + objectHeight; j++)
            if (isPosInMap(i, j))
                map[j][i] = obj.cType;
}

void putScoreOnMap(int score, char map[mapHeight][mapWidth + 1])
{
    char scoreText[30];
    sprintf(scoreText, "Score: %d", score);

    int len = strlen(scoreText);

    for (int i = 0; i < len; i++)
        map[1][i + 5] = scoreText[i];
}

void setCur(int x, int y) {
    move(y, x);
}

void setObjectPos(TObject* obj, float xPos, float yPos) {
    (*obj).x = xPos;
    (*obj).y = yPos;
}

void showMap(char map[mapHeight][mapWidth + 1]) {
    map[mapHeight - 1][mapWidth - 1] = '\0';

    for (int j = 0; j < mapHeight; j++)
        printw("%s\n", map[j]);
}

void vertMoveObject(TObject* obj, int* level, int* score, int maxLvl,
    TObject** brick, int* brickLength,
    TObject** moving, int* movingLength,
    TObject* mario,
    char map[mapHeight][mapWidth + 1]) {
    (void)map;

    (*obj).isFly = true;
    (*obj).vertSpeed += 0.05;

    setObjectPos(obj, (*obj).x, (*obj).y + (*obj).vertSpeed);

    for (int i = 0; i < *brickLength; i++)
        if (isCollision(*obj, (*brick)[i])) {

            if ((*obj).vertSpeed > 0)
                (*obj).isFly = false;

            if (((*brick)[i].cType == '?') && ((*obj).vertSpeed < 0) && (obj == mario))
            {
                (*brick)[i].cType = '-';
                initObject(getNewMoving(moving, movingLength), (*brick)[i].x, (*brick)[i].y - 3, 3, 2, '$');
                (*moving)[*movingLength - 1].vertSpeed = -0.7;
            }

            (*obj).y -= (*obj).vertSpeed;
            (*obj).vertSpeed = 0;

            if ((*brick)[i].cType == '+')
            {
                (*level)++;
                if ((*level) > maxLvl)
                    (*level) = 1;

                
                napms(500);
                createLevel(*level, score, level, maxLvl, brick, brickLength, moving, movingLength, mario);
                return;
            }

            break;
        }
}