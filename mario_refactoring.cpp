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

char map[MAP_HEIGHT][MAP_WIDTH + 1];
GameObject mario;

GameObject *brick = NULL;
int brickLength = 0;

GameObject *moving = NULL;
int movingLength = 0;

int level = 1;
int score = 0;

void ClearMap() {

    for (int i = 0; i < MAP_HEIGHT; i++){
        for (int j = 0; j < MAP_WIDTH; j++)
            map[i][j] = ' ';
        map[i][MAP_WIDTH]  = '\0';
    }
    
}

void ShowMap() {

    map[MAP_HEIGHT - 1][MAP_WIDTH - 1] = '\0';

    for (int j = 0; j < MAP_HEIGHT;j++)
        printf("%s\n", map[j]);
}

void SetObjectPos(GameObject *obj, float xPos, float yPos){

    (*obj).x=xPos;
    (*obj).y=yPos;
}

void InitObject(GameObject *obj, float xPos, float yPos, float oWidth, float oHeight, ObjectType inType){

    SetObjectPos(obj, xPos, yPos);
    (*obj).width = oWidth;
    (*obj).height = oHeight;
    (*obj).verticalSpeed = 0;
    (*obj).isFlying = false;
    (*obj).type = inType;


    (*obj).horizontalSpeed = 0.2;
}

void CreateLevel(int lvl);
bool IsCollision(GameObject o1, GameObject o2);
GameObject *GetNewMoving();
GameObject *ResizeObjects(GameObject *objects, int count);

void PlayerDead()
{
    napms(500);
    score = 0;
    CreateLevel(level);
}

void VertMoveObject(GameObject *obj){

    (*obj).isFlying = true;
    (*obj).verticalSpeed += 0.05;

    SetObjectPos (obj, (*obj).x, (*obj).y + (*obj).verticalSpeed);

    for (int i=0; i< brickLength; i++)
        if (IsCollision(*obj, brick[i])){

            if ((*obj).verticalSpeed > 0)
                (*obj).isFlying = false;


            if ((brick[i].type == ObjectType::questionBlock) && ((*obj).verticalSpeed < 0) && (obj == &mario))
            {
                brick[i].type = ObjectType::platform;
                InitObject(GetNewMoving(), brick[i].x, brick[i].y - 3, 3, 2, ObjectType::collectible);
                moving[movingLength - 1].verticalSpeed = -0.7;
            }

            (*obj).y -= (*obj).verticalSpeed;
            (*obj).verticalSpeed = 0;

            if (brick[i].type == ObjectType::goal)
            {
                level++;
                if (level > MAX_LEVEL) level = 1;

                napms(500);
                CreateLevel(level);
            }

            break;
        }
}

void DeleteMoving(int i) {
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

void MarioCollision()
{
    for (int i = 0; i < movingLength; i++)
        if (IsCollision(mario, moving[i]))
        {
            if (moving[i].type == ObjectType::enemy)
            {
                if ((mario.isFlying == true) &&
                    (mario.verticalSpeed > 0) &&
                    (mario.y + mario.height < moving[i].y + moving[i].height * 0.5))
                {
                    score += 50;
                    DeleteMoving(i);
                    i--;
                    continue;
                }

                PlayerDead();
            }

            if (moving[i].type == ObjectType::collectible)
            {
                score += 100;
                DeleteMoving(i);
                i--;
                continue;
            }
        }
}

void HorizonMoveObject(GameObject *obj)
{
    obj[0].x += obj[0].horizontalSpeed;

    for (int i = 0; i < brickLength; i++)
        if (IsCollision(obj[0], brick[i]))
        {
            obj[0].x -= obj[0].horizontalSpeed;
            obj[0].horizontalSpeed = -obj[0].horizontalSpeed;
            return;
        }

    if (obj[0].type == ObjectType::enemy)
    {
        GameObject tmp = *obj;
        VertMoveObject(&tmp);

        if (tmp.isFlying == true)
        {
            obj[0].x -= obj[0].horizontalSpeed;
            obj[0].horizontalSpeed = -obj[0].horizontalSpeed;
        }
    }
}

bool IsPosInMap(int x, int y){

    return ( (x >=0) && (x< MAP_WIDTH) && (y >= 0) && (y < MAP_HEIGHT) );
}

void PutObjectOnMap(GameObject obj){

    int ix = (int)round(obj.x);
    int iy = (int)round(obj.y);
    int iWidht = (int)round(obj.width);
    int iHeight = (int)round(obj.height);

    for (int i = ix; i < ix + iWidht; i++)
        for (int j = iy; j < iy +iHeight; j++)
            if (IsPosInMap(i,j))
                map[j][i] = static_cast<char>(obj.type);
}

void PutScoreOnMap()
{
    char c[30];
    sprintf(c, "Score: %d  Level: %d", score, level);

    int len = strlen(c);

    for (int i = 0; i < len; i++)
    {
        if (i + 2 < MAP_WIDTH)
            map[1][i + 2] = c[i];
}
}

void setCur (int x, int y){
    move(y, x);
}

void HorizonMoveMap(float dx){
    mario.x -= dx;

    for (int i = 0; i < brickLength; i ++)
        if (IsCollision(mario, brick[i])){
            mario.x +=dx;
            return;
        }

    mario.x += dx;

    for (int i = 0; i < brickLength; i++)
        brick[i].x +=dx;

    for (int i = 0; i < movingLength; i++)
        moving[i].x +=dx;
}

bool IsCollision(GameObject o1, GameObject o2){
    return ((o1.x + o1.width)> o2.x) && (o1.x < (o2.x + o2.width)) &&
           ((o1.y + o1.height)> o2.y) && (o1.y < (o2.y + o2.height));
}

GameObject *GetNewBrick()
{
    brickLength++;
    brick = ResizeObjects(brick, brickLength);
    return brick + brickLength - 1;
}

GameObject *GetNewMoving()
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

void CreateLevel(int lvl){

    brickLength = 0;
    brick = ResizeObjects(brick, 0);

    movingLength = 0;
    moving = ResizeObjects(moving, 0);

    InitObject(&mario, 39, 10, 3, 3, ObjectType::player);

    if (lvl == 1)
    {
        InitObject(GetNewBrick(), 20, 20, 40, 5, ObjectType::brick);
        InitObject(GetNewBrick(), 30, 10, 5, 3, ObjectType::questionBlock);
        InitObject(GetNewBrick(), 50, 10, 5, 3, ObjectType::questionBlock);
        InitObject(GetNewBrick(), 60, 15, 40, 10, ObjectType::brick);
        InitObject(GetNewBrick(), 60, 5, 10, 3, ObjectType::platform);
        InitObject(GetNewBrick(), 70, 5, 5, 3, ObjectType::questionBlock);
        InitObject(GetNewBrick(), 75, 5, 5, 3, ObjectType::platform);
        InitObject(GetNewBrick(), 80, 5, 5, 3, ObjectType::questionBlock);
        InitObject(GetNewBrick(), 85, 5, 10, 3, ObjectType::platform);
        InitObject(GetNewBrick(), 100, 20, 20, 5, ObjectType::brick);
        InitObject(GetNewBrick(), 120, 15, 10, 10, ObjectType::brick);
        InitObject(GetNewBrick(), 150, 20, 40, 5, ObjectType::brick);
        InitObject(GetNewBrick(), 210, 15, 10, 10, ObjectType::goal);

        InitObject(GetNewMoving(), 25, 10, 3, 2, ObjectType::enemy);
        InitObject(GetNewMoving(), 80, 10, 3, 2, ObjectType::enemy);
    }

    if (lvl == 2)
    {
        InitObject(GetNewBrick(), 20, 20, 40, 5, ObjectType::brick);
        InitObject(GetNewBrick(), 60, 15, 10, 10, ObjectType::brick);
        InitObject(GetNewBrick(), 80, 20, 20, 5, ObjectType::brick);
        InitObject(GetNewBrick(), 120, 15, 10, 10, ObjectType::brick);
        InitObject(GetNewBrick(), 150, 20, 40, 5, ObjectType::brick);
        InitObject(GetNewBrick(), 210, 15, 10, 10, ObjectType::goal);

        InitObject(GetNewMoving(), 25, 10, 3, 2, ObjectType::enemy);
        InitObject(GetNewMoving(), 80, 10, 3, 2, ObjectType::enemy);
        InitObject(GetNewMoving(), 65, 10, 3, 2, ObjectType::enemy);
        InitObject(GetNewMoving(), 120, 10, 3, 2, ObjectType::enemy);
        InitObject(GetNewMoving(), 160, 10, 3, 2, ObjectType::enemy);
        InitObject(GetNewMoving(), 175, 10, 3, 2, ObjectType::enemy);
    }

    if (lvl == 3)
    {
        InitObject(GetNewBrick(), 20, 20, 40, 5, ObjectType::brick);
        InitObject(GetNewBrick(), 80, 20, 15, 5, ObjectType::brick);
        InitObject(GetNewBrick(), 120, 15, 15, 10, ObjectType::brick);
        InitObject(GetNewBrick(), 160, 10, 15, 15, ObjectType::goal);

        InitObject(GetNewMoving(), 25, 10, 3, 2, ObjectType::enemy);
        InitObject(GetNewMoving(), 50, 10, 3, 2, ObjectType::enemy);
        InitObject(GetNewMoving(), 80, 10, 3, 2, ObjectType::enemy);
        InitObject(GetNewMoving(), 90, 10, 3, 2, ObjectType::enemy);
        InitObject(GetNewMoving(), 120, 10, 3, 2, ObjectType::enemy);
        InitObject(GetNewMoving(), 130, 10, 3, 2, ObjectType::enemy);
    }
}

int main() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    timeout(0);
    CreateLevel(level);

    
    bool running = true;

    while (running)
    {
        ClearMap();
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
                HorizonMoveMap(2.0f);
            }

            if (key == 'd' || key == 'D' || key == KEY_RIGHT)
            {
                HorizonMoveMap(-2.0f);
            }

            if (key == ' ' && mario.isFlying == false)
            {
                mario.verticalSpeed = -1.2f;
            }
        }

        if (!running)
            break;

        if (mario.y > MAP_HEIGHT)
            PlayerDead();

        VertMoveObject(&mario);

        MarioCollision();

        for (int i = 0; i < brickLength; i++)
            PutObjectOnMap(brick[i]);

        for (int i = 0; i < movingLength; i++)
        {
            VertMoveObject(moving + i);
            HorizonMoveObject(moving + i);

            if (moving[i].y > MAP_HEIGHT)
            {
                DeleteMoving(i);
                i--;
                continue;
            }

            PutObjectOnMap(moving[i]);
        }

        PutObjectOnMap(mario);
        PutScoreOnMap();

        move(0, 0);

        for (int j = 0; j < MAP_HEIGHT; j++)
            printw("%s\n", map[j]);

        refresh();

        usleep(16000);
    }

    free(brick);
    free(moving);

    endwin();

    return 0;
}