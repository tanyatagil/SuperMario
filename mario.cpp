#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <math.h>
#include <windows.h>
#include <string.h>

#define mapWidth 80
#define mapHeight 25

typedef struct SObject{
    float x,y; 
    float width, height;
    float vertSpeed;

} TObject;

char map[mapHeight][mapWidth + 1];
TObject mario;


void ClearMap() {

    for (int i = 0; i < mapWidth; i++)
        map[0][i] = ' ';

    map[0][mapWidth] = '\0';

    for (int j = 1; j < mapHeight; j++)
        sprintf(map[j], map[0]);
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


void initObject(TObject* obj, float xPos, float yPos, float objectWidth, float objectHeight, char inType) {
    SetObjectPos(obj, xPos, yPos);
    (*obj).width = objectWidth;
    (*obj).height = objectHeight;
    (*obj).vertSpeed = 0;
}

void PutObjectOnMap(TObject obj){

    int ix = (int)round(obj.x);
    int iy = (int)round(obj.y);
    int iWidht = (int)round(obj.width);
    int iHeight = (int)round(obj.height);

    for (int i = ix; i < ix + iWidht; i++)
        for (int j = iy; j < iy +iHeight; j++)
            map[j][i] = '0';
}

void setCur (int x, int y){
    COORD coord;
    coord.X =x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
BOOL IsCollision(TObject o1, TObject o2){
    return ((o1.x + o1.width)> o2.x) && (o1.x < (o2.x + o2.width)) &&
           ((o1.y + o1.height)> o2.y) && (o1.y < (o2.y + o2.height));
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

int main()
{
    CreateLevel(level);

    do
    {
        ClearMap();if ((mario.IsFly == FALSE) && (GetKeyState(VK_SPACE) < 0)) mario.vertSpeed = -1;
        if(GetKeyState ('A') < 0) HorizonMoveMap(1);
        if(GetKeyState ('D') < 0) HorizonMoveMap(-1);

        if (mario.y > mapHeight) PlayerDead();

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

        setCur(0,0);
        ShowMap();

        Sleep(10);

    }
    while (GetKeyState(VK_ESCAPE)>=0);

    free(brick);
    free(moving);

    return 0;
}