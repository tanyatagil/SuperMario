#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>

#define mapWidth 80
#define mapHeight 25

typedef struct SObject{
    float x,y;
    float width, height;
    float vertSpeed;
    bool IsFly;
    char cType;


    float horizSpeed;

} TObject;

char map[mapHeight][mapWidth + 1];
TObject mario;

TObject *brick = NULL;
int brickLength = 0;

TObject *moving = NULL;
int movingLength = 0;

int level = 1;
int score = 0;
int maxLvl = 3;

void ClearMap() {

    for (int i = 0; i < mapHeight; i++){
        for (int j = 0; j < mapWidth; j++)
            map[i][j] = ' ';
        map[i][mapWidth]  = '\0';
    }
    
}

void ShowMap() {

    map[mapHeight - 1][mapWidth - 1] = '\0';

    for (int j = 0; j < mapHeight;j++)
        printf("%s\n", map[j]);
}

void SetObjectPos(TObject *obj, float xPos, float yPos){

    (*obj).x=xPos;
    (*obj).y=yPos;
}

void InitObject(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, char inType){

    SetObjectPos(obj, xPos, yPos);
    (*obj).width = oWidth;
    (*obj).height = oHeight;
    (*obj).vertSpeed = 0;
    (*obj).IsFly = false;
    (*obj).cType = inType;


    (*obj).horizSpeed = 0.2;
}

void CreateLevel(int lvl);
bool IsCollision(TObject o1, TObject o2);
TObject *GetNewMoving();

void PlayerDead()
{
    napms(500);
    CreateLevel(level);
}

void VertMoveObject(TObject *obj){

    (*obj).IsFly = true;
    (*obj).vertSpeed += 0.05;

    SetObjectPos (obj, (*obj).x, (*obj).y + (*obj).vertSpeed);

    for (int i=0; i< brickLength; i++)
        if (IsCollision(*obj, brick[i])){

            if ((*obj).vertSpeed > 0)
                (*obj).IsFly = false;


            if ((brick[i].cType == '?') && ((*obj).vertSpeed < 0) && (obj == &mario))
            {
                brick[i].cType = '-';
                InitObject(GetNewMoving(), brick[i].x, brick[i].y - 3, 3, 2, '$');
                moving[movingLength - 1].vertSpeed = -0.7;
            }

            (*obj).y -= (*obj).vertSpeed;
            (*obj).vertSpeed = 0;


            if (brick[i].cType == '+')
            {
                level++;
                if (level > maxLvl) level = 1;

                napms(500);
                CreateLevel(level);
            }

            break;
        }
}

void DeleteMoving(int i)
{
    movingLength--;
    moving[i] = moving[movingLength];
    moving = (TObject*)realloc(moving, sizeof(*moving) * movingLength);
}

void MarioCollision()
{
    for (int i = 0; i < movingLength; i++)
        if (IsCollision(mario, moving[i]))
        {
            if (moving[i].cType == 'o')
            {
                if ((mario.IsFly == true) &&
                    (mario.vertSpeed > 0) &&
                    (mario.y + mario.height < moving[i].y + moving[i].height * 0.5))
                {
                    score += 50;
                    DeleteMoving(i);
                    i--;
                    continue;
                }

                PlayerDead();
            }

            if (moving[i].cType == '$')
            {
                score += 100;
                DeleteMoving(i);
                i--;
                continue;
            }
        }
}

void HorizonMoveObject(TObject *obj)
{
    obj[0].x += obj[0].horizSpeed;

    for (int i = 0; i < brickLength; i++)
        if (IsCollision(obj[0], brick[i]))
        {
            obj[0].x -= obj[0].horizSpeed;
            obj[0].horizSpeed = -obj[0].horizSpeed;
            return;
        }

    if (obj[0].cType == 'o')
    {
        TObject tmp = *obj;
        VertMoveObject(&tmp);

        if (tmp.IsFly == true)
        {
            obj[0].x -= obj[0].horizSpeed;
            obj[0].horizSpeed = -obj[0].horizSpeed;
        }
    }
}

bool IsPosInMap(int x, int y){

    return ( (x >=0) && (x< mapWidth) && (y >= 0) && (y < mapHeight) );
}

void PutObjectOnMap(TObject obj){

    int ix = (int)round(obj.x);
    int iy = (int)round(obj.y);
    int iWidht = (int)round(obj.width);
    int iHeight = (int)round(obj.height);

    for (int i = ix; i < ix + iWidht; i++)
        for (int j = iy; j < iy +iHeight; j++)
            if (IsPosInMap(i,j))
                map[j][i] = obj.cType;
}

void PutScoreOnMap()
{
    char c[30];
    sprintf(c, "Score: %d", score);

    int len = strlen(c);

    for (int i = 0; i < len; i++)
        map[1][i + 5] = c[i];
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

bool IsCollision(TObject o1, TObject o2){
    return ((o1.x + o1.width)> o2.x) && (o1.x < (o2.x + o2.width)) &&
           ((o1.y + o1.height)> o2.y) && (o1.y < (o2.y + o2.height));
}

TObject *GetNewBrick()
{
    brickLength++;
    brick = (TObject*)realloc(brick, sizeof(*brick) * brickLength);
    return brick + brickLength - 1;
}

TObject *GetNewMoving()
{
    movingLength++;
    moving = (TObject*)realloc(moving, sizeof(*moving) * movingLength);
    return moving + movingLength - 1;
}

void CreateLevel(int lvl){

    system("color 9F");

    brickLength = 0;
    brick = (TObject*)realloc(brick, 0);

    movingLength = 0;
    moving = (TObject*)realloc(moving, 0);

    InitObject(&mario, 39, 10, 3, 3, '@');

    if (lvl == 1)
    {
        InitObject(GetNewBrick(), 20, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 30, 10, 5, 3, '?');
        InitObject(GetNewBrick(), 50, 10, 5, 3, '?');
        InitObject(GetNewBrick(), 60, 15, 40, 10, '#');
        InitObject(GetNewBrick(), 60, 5, 10, 3, '-');
        InitObject(GetNewBrick(), 70, 5, 5, 3, '?');
        InitObject(GetNewBrick(), 75, 5, 5, 3, '-');
        InitObject(GetNewBrick(), 80, 5, 5, 3, '?');
        InitObject(GetNewBrick(), 85, 5, 10, 3, '-');
        InitObject(GetNewBrick(), 100, 20, 20, 5, '#');
        InitObject(GetNewBrick(), 120, 15, 10, 10, '#');
        InitObject(GetNewBrick(), 150, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 210, 15, 10, 10, '+');

        InitObject(GetNewMoving(), 25, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 80, 10, 3, 2, 'o');
    }

    if (lvl == 2)
    {
        InitObject(GetNewBrick(), 20, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 60, 15, 10, 10, '#');
        InitObject(GetNewBrick(), 80, 20, 20, 5, '#');
        InitObject(GetNewBrick(), 120, 15, 10, 10, '#');
        InitObject(GetNewBrick(), 150, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 210, 15, 10, 10, '+');

        InitObject(GetNewMoving(), 25, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 80, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 65, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 120, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 160, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 175, 10, 3, 2, 'o');
    }

    if (lvl == 3)
    {
        InitObject(GetNewBrick(), 20, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 80, 20, 15, 5, '#');
        InitObject(GetNewBrick(), 120, 15, 15, 10, '#');
        InitObject(GetNewBrick(), 160, 10, 15, 15, '+');

        InitObject(GetNewMoving(), 25, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 50, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 80, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 90, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 120, 10, 3, 2, 'o');
        InitObject(GetNewMoving(), 130, 10, 3, 2, 'o');
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

            if (key == ' ' && mario.IsFly == false)
            {
                mario.vertSpeed = -1.2f;
            }
        }

        if (!running)
            break;

        if (mario.y > mapHeight)
            PlayerDead();

        VertMoveObject(&mario);

        MarioCollision();

        for (int i = 0; i < brickLength; i++)
            PutObjectOnMap(brick[i]);

        for (int i = 0; i < movingLength; i++)
        {
            VertMoveObject(moving + i);
            HorizonMoveObject(moving + i);

            if (moving[i].y > mapHeight)
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

        for (int j = 0; j < mapHeight; j++)
            printw("%s\n", map[j]);

        refresh();

        usleep(16000);
    }

    free(brick);
    free(moving);

    return 0;
}