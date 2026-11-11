#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>

#define MAP_WIDTH 80
#define MAP_HEIGHT 25
#define MAX_LEVEL 3

enum class ObjectType : char {
    player = '@',
    brick = '#',
    questionBlock = '?',
    platform = '-',
    goal = '+',
    enemy = 'o',
    collectible = '$'
};

typedef struct GameObject {
    float x,y;
    float width, height;
    float verticalSpeed;
    bool isFlying;
    ObjectType type;


    float horizontalSpeed;

} GameObject;

GameObject mario;

GameObject *brick = NULL;
int brickLength = 0;

GameObject *moving = NULL;
int movingLength = 0;

int level = 1;
int score = 0;

void clearMap(char currentMap[MAP_HEIGHT][MAP_WIDTH + 1]) {

    for (int i = 0; i < MAP_HEIGHT; i++){
        for (int j = 0; j < MAP_WIDTH; j++)
            currentMap[i][j] = ' ';
        currentMap[i][MAP_WIDTH]  = '\0';
    }
    
}

void showMap(char currentMap[MAP_HEIGHT][MAP_WIDTH + 1]) {

    currentMap[MAP_HEIGHT - 1][MAP_WIDTH - 1] = '\0';

    for (int j = 0; j < MAP_HEIGHT;j++)
        printf("%s\n", currentMap[j]);
}

void setObjectPos(GameObject *obj, float xPos, float yPos){

    (*obj).x=xPos;
    (*obj).y=yPos;
}

void initObject(GameObject *obj, float xPos, float yPos, float oWidth, float oHeight, ObjectType inType){

    setObjectPos(obj, xPos, yPos);
    (*obj).width = oWidth;
    (*obj).height = oHeight;
    (*obj).verticalSpeed = 0;
    (*obj).isFlying = false;
    (*obj).type = inType;


    (*obj).horizontalSpeed = 0.2;
}

void createLevel(int lvl);
bool isCollision(GameObject o1, GameObject o2);
GameObject *getNewMoving();
GameObject *ResizeObjects(GameObject *objects, int count);

void playerDead()
{
    napms(500);
    score = 0;
    createLevel(level);
}

void vertMoveObject(GameObject *obj){

    (*obj).isFlying = true;
    (*obj).verticalSpeed += 0.05;

    setObjectPos (obj, (*obj).x, (*obj).y + (*obj).verticalSpeed);

    for (int i=0; i< brickLength; i++)
        if (isCollision(*obj, brick[i])){

            if ((*obj).verticalSpeed > 0)
                (*obj).isFlying = false;


            if ((brick[i].type == ObjectType::questionBlock) && ((*obj).verticalSpeed < 0) && (obj == &mario))
            {
                brick[i].type = ObjectType::platform;
                initObject(getNewMoving(), brick[i].x, brick[i].y - 3, 3, 2, ObjectType::collectible);
                moving[movingLength - 1].verticalSpeed = -0.7;
            }

            (*obj).y -= (*obj).verticalSpeed;
            (*obj).verticalSpeed = 0;

            if (brick[i].type == ObjectType::goal)
            {
                level++;
                if (level > MAX_LEVEL) level = 1;

                napms(500);
                createLevel(level);
            }

            break;
        }
}

void deleteMoving(int i) {
    if (movingLength <= 0)
        return;

    movingLength--;

    if (i != movingLength)
        moving[i] = moving[movingLength];

    if (movingLength == 0)
    {
        free(moving);
        moving = NULL;
    }
    else
    {
        moving = ResizeObjects(moving, movingLength);
    }
}

void marioCollision()
{
    for (int i = 0; i < movingLength; i++)
        if (isCollision(mario, moving[i]))
        {
            if (moving[i].type == ObjectType::enemy)
            {
                if ((mario.isFlying == true) &&
                    (mario.verticalSpeed > 0) &&
                    (mario.y + mario.height < moving[i].y + moving[i].height * 0.5))
                {
                    score += 50;
                    deleteMoving(i);
                    i--;
                    continue;
                }

                playerDead();
            }

            if (moving[i].type == ObjectType::collectible)
            {
                score += 100;
                deleteMoving(i);
                i--;
                continue;
            }
        }
}

void horizonMoveObject(GameObject *obj)
{
    obj[0].x += obj[0].horizontalSpeed;

    for (int i = 0; i < brickLength; i++)
        if (isCollision(obj[0], brick[i]))
        {
            obj[0].x -= obj[0].horizontalSpeed;
            obj[0].horizontalSpeed = -obj[0].horizontalSpeed;
            return;
        }

    if (obj[0].type == ObjectType::enemy)
    {
        GameObject tmp = *obj;
        vertMoveObject(&tmp);

        if (tmp.isFlying == true)
        {
            obj[0].x -= obj[0].horizontalSpeed;
            obj[0].horizontalSpeed = -obj[0].horizontalSpeed;
        }
    }
}

bool isPosInMap(int x, int y){

    return ( (x >=0) && (x< MAP_WIDTH) && (y >= 0) && (y < MAP_HEIGHT) );
}

void putObjectOnMap(GameObject obj, char currentMap[MAP_HEIGHT][MAP_WIDTH + 1]){

    int ix = (int)round(obj.x);
    int iy = (int)round(obj.y);
    int iWidht = (int)round(obj.width);
    int iHeight = (int)round(obj.height);

    for (int i = ix; i < ix + iWidht; i++)
        for (int j = iy; j < iy +iHeight; j++)
            if (isPosInMap(i,j))
                currentMap[j][i] = static_cast<char>(obj.type);
}

void putScoreOnMap(int currentScore, char currentMap[MAP_HEIGHT][MAP_WIDTH + 1])
{
    char c[30];
    sprintf(c, "Score: %d  Level: %d", currentScore, level);

    int len = strlen(c);

    for (int i = 0; i < len; i++)
    {
        if (i + 2 < MAP_WIDTH)
            currentMap[1][i + 2] = c[i];
}
}

void setCur (int x, int y){
    move(y, x);
}

void horizonMoveMap(float dx){
    mario.x -= dx;

    for (int i = 0; i < brickLength; i ++)
        if (isCollision(mario, brick[i])){
            mario.x +=dx;
            return;
        }

    mario.x += dx;

    for (int i = 0; i < brickLength; i++)
        brick[i].x +=dx;

    for (int i = 0; i < movingLength; i++)
        moving[i].x +=dx;
}

bool isCollision(GameObject o1, GameObject o2){
    return ((o1.x + o1.width)> o2.x) && (o1.x < (o2.x + o2.width)) &&
           ((o1.y + o1.height)> o2.y) && (o1.y < (o2.y + o2.height));
}

GameObject *getNewBrick()
{
    brickLength++;
    brick = ResizeObjects(brick, brickLength);
    return brick + brickLength - 1;
}

GameObject *getNewMoving()
{
    movingLength++;
    moving = ResizeObjects(moving, movingLength);
    return moving + movingLength - 1;
}

GameObject *ResizeObjects(GameObject *objects, int count)
{
    if (count == 0)
    {
        free(objects);
        return NULL;
    }

    GameObject *resized = (GameObject*)realloc(
        objects,
        sizeof(GameObject) * count
    );

    if (resized == NULL)
    {
        perror("Failed to resize game objects");
        exit(EXIT_FAILURE);
    }

    return resized;
}

void createLevel(int lvl){

    brickLength = 0;
    brick = ResizeObjects(brick, 0);

    movingLength = 0;
    moving = ResizeObjects(moving, 0);

    initObject(&mario, 39, 10, 3, 3, ObjectType::player);

    if (lvl == 1)
    {
        initObject(getNewBrick(), 20, 20, 40, 5, ObjectType::brick);
        initObject(getNewBrick(), 30, 10, 5, 3, ObjectType::questionBlock);
        initObject(getNewBrick(), 50, 10, 5, 3, ObjectType::questionBlock);
        initObject(getNewBrick(), 60, 15, 40, 10, ObjectType::brick);
        initObject(getNewBrick(), 60, 5, 10, 3, ObjectType::platform);
        initObject(getNewBrick(), 70, 5, 5, 3, ObjectType::questionBlock);
        initObject(getNewBrick(), 75, 5, 5, 3, ObjectType::platform);
        initObject(getNewBrick(), 80, 5, 5, 3, ObjectType::questionBlock);
        initObject(getNewBrick(), 85, 5, 10, 3, ObjectType::platform);
        initObject(getNewBrick(), 100, 20, 20, 5, ObjectType::brick);
        initObject(getNewBrick(), 120, 15, 10, 10, ObjectType::brick);
        initObject(getNewBrick(), 150, 20, 40, 5, ObjectType::brick);
        initObject(getNewBrick(), 210, 15, 10, 10, ObjectType::goal);

        initObject(getNewMoving(), 25, 10, 3, 2, ObjectType::enemy);
        initObject(getNewMoving(), 80, 10, 3, 2, ObjectType::enemy);
    }

    if (lvl == 2)
    {
        initObject(getNewBrick(), 20, 20, 40, 5, ObjectType::brick);
        initObject(getNewBrick(), 60, 15, 10, 10, ObjectType::brick);
        initObject(getNewBrick(), 80, 20, 20, 5, ObjectType::brick);
        initObject(getNewBrick(), 120, 15, 10, 10, ObjectType::brick);
        initObject(getNewBrick(), 150, 20, 40, 5, ObjectType::brick);
        initObject(getNewBrick(), 210, 15, 10, 10, ObjectType::goal);

        initObject(getNewMoving(), 25, 10, 3, 2, ObjectType::enemy);
        initObject(getNewMoving(), 80, 10, 3, 2, ObjectType::enemy);
        initObject(getNewMoving(), 65, 10, 3, 2, ObjectType::enemy);
        initObject(getNewMoving(), 120, 10, 3, 2, ObjectType::enemy);
        initObject(getNewMoving(), 160, 10, 3, 2, ObjectType::enemy);
        initObject(getNewMoving(), 175, 10, 3, 2, ObjectType::enemy);
    }

    if (lvl == 3)
    {
        initObject(getNewBrick(), 20, 20, 40, 5, ObjectType::brick);
        initObject(getNewBrick(), 80, 20, 15, 5, ObjectType::brick);
        initObject(getNewBrick(), 120, 15, 15, 10, ObjectType::brick);
        initObject(getNewBrick(), 160, 10, 15, 15, ObjectType::goal);

        initObject(getNewMoving(), 25, 10, 3, 2, ObjectType::enemy);
        initObject(getNewMoving(), 50, 10, 3, 2, ObjectType::enemy);
        initObject(getNewMoving(), 80, 10, 3, 2, ObjectType::enemy);
        initObject(getNewMoving(), 90, 10, 3, 2, ObjectType::enemy);
        initObject(getNewMoving(), 120, 10, 3, 2, ObjectType::enemy);
        initObject(getNewMoving(), 130, 10, 3, 2, ObjectType::enemy);
    }
}

int main() {
    char currentMap[MAP_HEIGHT][MAP_WIDTH + 1];

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    timeout(0);
    createLevel(level);

    
    bool running = true;

    while (running)
    {
        clearMap(currentMap);
        int key;

        while ((key = getch()) != ERR)
        {
            if (key == 27)
            {
                running = false;
                break;
            }

            if (key == 'a' || key == 'A' || key == KEY_LEFT)
            {
                horizonMoveMap(2.0f);
            }

            if (key == 'd' || key == 'D' || key == KEY_RIGHT)
            {
                horizonMoveMap(-2.0f);
            }

            if (key == ' ' && mario.isFlying == false)
            {
                mario.verticalSpeed = -1.2f;
            }
        }

        if (!running)
            break;

        if (mario.y > MAP_HEIGHT)
            playerDead();

        vertMoveObject(&mario);

        marioCollision();

        for (int i = 0; i < brickLength; i++)
            putObjectOnMap(brick[i], currentMap);

        for (int i = 0; i < movingLength; i++)
        {
            vertMoveObject(moving + i);
            horizonMoveObject(moving + i);

            if (moving[i].y > MAP_HEIGHT)
            {
                deleteMoving(i);
                i--;
                continue;
            }

            putObjectOnMap(moving[i], currentMap);
        }

        putObjectOnMap(mario, currentMap);
        putScoreOnMap(score, currentMap);

        move(0, 0);

        for (int j = 0; j < MAP_HEIGHT; j++)
            printw("%s\n", currentMap[j]);

        refresh();

        usleep(16000);
    }

    free(brick);
    free(moving);

    endwin();

    return 0;
}