#include <stdbool.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define mapWidth 80
#define mapHeight 25
#define KEY_ESC 27
#define KEY_SPACE 32
#define CREEPS 0
#define JUMPING 1
#define FLIES 2

void lag(int stage)
{
    for (int i = 0; i < stage; i++)
    {
        ;
    }
    
}

typedef struct
{
    float x, y;
    float width, height;
    float vertSpeed;
    bool isFly;
    char type;
    float horizontalSpeed;
    int skill;
} object;


char map[mapHeight][mapWidth+1];
object mario;
object *brick = NULL;
object *monster = NULL;
int brickSum;
int monsterSum;
int level = 1;
int maxLevel;
int score=0;
char statOnMap[2][15];


void сlearMap()
{
    for (int i = 0; i < mapWidth; i++)
    {
        map[0][i] = '*';
        map[1][i] = ' ';
    }
    map[0][mapWidth] = '\0';
    for (int j = 1; j < mapHeight; j++)
    {
            sprintf(map[j], map[1], 0);
            map[j][0] = '*';
            map[j][mapWidth-1] = '*';
    }
            
    
}

void showMap()
{   
    for (int i = 0; i < mapHeight; i++)
    {
        addstr(map[i]);
        printw("\n");
    }
    refresh();
}

void setObjectPos(object *obj, float xPos, float yPos)
{
    (*obj).x = xPos;
    (*obj).y = yPos;
}

void initObject(object *obj, float xPos, float yPos, float objWidth, float objHeight, char objType)
{
    setObjectPos(obj, xPos, yPos);
    obj->width = objWidth;
    obj->height = objHeight;
    obj->vertSpeed = 0;
    obj->type = objType;
    obj->horizontalSpeed = 0.2;
}

bool isCollision(object obj1, object obj2)
{
     return ((obj1.x + obj1.width) > obj2.x) && (obj1.x < (obj2.x + obj2.width)) && 
            ((obj1.y + obj1.height) > obj2.y) && (obj1.y < (obj2.y + obj2.height));
}

void createLevel(int lvl);

void deleteBrick(int *i)
{
    brickSum --;
    brick[*i] = brick [brickSum];
    brick = realloc(brick, sizeof(*brick)*brickSum);
    (*i)--;
}

void deleteMonster(int *i)
{
    monsterSum --;
    monster[*i] = monster [monsterSum];
    monster = realloc(monster, sizeof(*monster)*monsterSum);
    (*i)--;
}

void marioDead()
{
    level = 1;
    score = 0;
    lag(300000000);
    createLevel(level);
}

void vertMoveObject(object *obj)
{
    (*obj).isFly = true;
    (*obj).vertSpeed += 0.05;
    setObjectPos(obj, (*obj).x, (*obj).y + (*obj).vertSpeed);
    
    for (int i = 0; i < brickSum; i++)
    {
        if (isCollision(*obj, brick[i]))
        {
            (*obj).y -= (*obj).vertSpeed;
            (*obj).vertSpeed = 0;
            (*obj).isFly = false;

            if (brick[i].type == '$')
            {
                deleteBrick(&i);
                score+=50;
                break;
            }

            if (brick[i].type == '+')
            {
                lag(300000000);
                level ++;
                if (level > maxLevel) level = 1;
                createLevel(level);
            }
            
            break;
        }
    }
}

bool isPosInMap(int x, int y)
{
    return ((x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight));
}

void putObjectOnMap(object obj)
{
    int ix = (int)round(obj.x);
    int iy = (int)round(obj.y);
    int iWidth = (int)round(obj.width);
    int iHeight = (int)round(obj.height);
    


   for (int i = ix; i < (ix + iWidth) ; i++)
   {
       for (int j = iy; j < (iy + iHeight); j++)
       {
           if(isPosInMap(i, j))
           {
                map[j][i] = obj.type;
           }
       }
   }
}

void horizontMoveObject(object *obj)
{
    obj->x += obj->horizontalSpeed;
    
    for (int i = 0; i < brickSum; i++)
    {
        if (isCollision(*obj, brick[i]))
        {   
            obj->x -= obj->horizontalSpeed;
            obj->horizontalSpeed = -(obj->horizontalSpeed);
            break;
        }
    }
    
    object tmp = *obj;


    for (int i = 0; i < brickSum; i++)
    {
        if(obj->isFly && (obj->y+obj->height) > brick[0].y)
        {
            obj->y = (brick[0].y - obj->height);
            obj->x -= obj->horizontalSpeed;
            obj->horizontalSpeed = -(obj->horizontalSpeed); 
        }    
    }

    //if(isCollision(*obj, mario) && !mario.isFly) deleteMonster();
/*
    for (int i = 0; i < monsterSum; i++)
    {
        if (isCollision(*obj, monster[i]))
        {
            deleteMonster(&i);
            score+=100;
            break;
        }
    }
*/
}
    
    
    

    

void horizontMoveMap(float dx)
{
    mario.x -= dx;

    for (int i = 0; i < brickSum; i++)
    {
        if (isCollision(mario, brick[i]))
        {
            if (brick[i].type == '$')
            {
                deleteBrick(&i);
                score+=50;
                break;
            }

            mario.x += dx;
            return;
        }
        
    }

    mario.x += dx;
    
    
    for (int i = 0; i < brickSum; i++)
    {
        brick[i].x += dx;
    }
    for (int i = 0; i < monsterSum; i++)
    {
        monster[i].x += dx;
    }
}

object *getNewBrick()
{
    brickSum++;
    brick = realloc(brick, sizeof(*brick) * brickSum);
    return brick + brickSum - 1;
}

object *getNewMonster()
{
    monsterSum++;
    monster = realloc(monster, sizeof(*monster) * monsterSum);
    return monster + monsterSum - 1;
}

void createLevel(int lvl)
{
    brickSum = 0;
    brick = realloc(brick, 0);
    monsterSum = 0;
    monster = realloc(monster, 0);


    initObject(&mario, 39, 2, 3, 3, '@');

    if (lvl == 2)
    {
    initObject(getNewBrick(), 26, 20, 38, 5, '#');
    initObject(getNewBrick(), 78, 13, 6, 5, '#');
    initObject(getNewBrick(), 84, 14, 6, 5, '#');
    initObject(getNewBrick(), 90, 15, 6, 5, '#');
    initObject(getNewBrick(), 97, 10, 8, 5, '#');
    initObject(getNewBrick(), 115, 8, 15, 17, '#');
    initObject(getNewBrick(), 120, 6, 2, 2, '$');
    initObject(getNewBrick(), 145, 20, 30, 5, '#');
    initObject(getNewBrick(), 168, 12, 4, 4, '#');
    initObject(getNewBrick(), 177, 10, 6, 5, '#');
    initObject(getNewBrick(), 190, 5, 15, 20, '#');
    initObject(getNewBrick(), 200, 3, 2, 2, '$');
    initObject(getNewBrick(), 215, 13, 40, 7, '#');
    initObject(getNewBrick(), 265, 10, 30, 4, '#');
    initObject(getNewBrick(), 280, 8, 2, 2, '$');
    initObject(getNewBrick(), 300, 9, 5, 5, '#');
    initObject(getNewBrick(), 310, 8, 5, 17, '#');
    initObject(getNewBrick(), 325, 13, 15, 12, '#');
    initObject(getNewBrick(), 355, 8, 10, 10, '+');
    }
    
    if (lvl == 1)
    {
    initObject(getNewBrick(), 20, 20, 40, 5, '#');
    initObject(getNewBrick(), 60, 15, 5, 10, '#');
    initObject(getNewMonster(), 25, 15, 2, 2, '0');
    initObject(getNewBrick(), 75, 15, 30, 5, '#');
    initObject(getNewMonster(), 95, 10, 2, 2, '0');

    }

    maxLevel = 2;
}

void putStatOnMap()
{
    sprintf(statOnMap[0], "Level: %d", level);
    sprintf(statOnMap[1], "Score: %d", score);

    int len = strlen(statOnMap[0]);
    for (int i = 0; i < len; i++)
    {
        map[1][i+2] = statOnMap[0][i];
    }

    len = strlen(statOnMap[1]);
    for (int i = 0; i < len; i++)
    {
        map[2][i+2] = statOnMap[1][i];
    }  
}

int main ()
{
    int key;
    int key2 = 0;
    bool onKeyRight = false;
    bool onKeyLeft = false;
    int r = 50;
    int i = 0;
    float l = 1;
    float g;
    initscr();
    //start_color();
    //init_pair(1,COLOR_BLACK, COLOR_BLUE);
    //wbkgd(stdscr, COLOR_PAIR(1));
    nodelay(stdscr, true);
    keypad(stdscr, true);
    noecho();
    raw();
    curs_set(0);

    createLevel(level);
    
    
    do
    {
        сlearMap();
        key = getch();

        
        if (key == KEY_RIGHT && i < r)
        {
            onKeyRight = true;
            onKeyLeft = false;
            i = 0;
            if (l < 1.5) l += 0.1;
        }
        
        if (key == KEY_LEFT && i < r)
        {
            onKeyRight = false;
            onKeyLeft = true;
            i = 0;
            if (l < 1.5) l += 0.1;
        }

        if (i < r) i++;

        if (i >= r)
        {
            onKeyRight = false;
            onKeyLeft = false;
            i = 0;
        }

        

        if (onKeyRight && (key == KEY_UP || key == KEY_SPACE)) 
        {
            key2 = KEY_RIGHT;
            g = 0.7*l;
        }
        if (onKeyLeft && (key == KEY_UP || key == KEY_SPACE)) 
        {
            key2 = KEY_LEFT;
            g = 0.7*l;
        }
    

        
        


        if ((!mario.isFly) && (key == KEY_UP || key == KEY_SPACE)) mario.vertSpeed = -1;
        if (key == KEY_RIGHT) horizontMoveMap(-3);
        if (key2 == KEY_RIGHT) 
        {   
            horizontMoveMap(-(g));
            g -= 0.023;
            if(g < 0) key2 = 0, l = 1;
        }

        if (key == KEY_LEFT) horizontMoveMap(3);
        if (key2 == KEY_LEFT) 
        {
            horizontMoveMap(g); 
            g -= 0.023;
            if(g < 0) key2 = 0, l = 1;  
        }

        if (mario.y > mapHeight) marioDead();
        
        vertMoveObject(&mario);

        for (int i = 0; i < brickSum; i++)
        {
            putObjectOnMap(brick[i]);
        }
        
        for (int i = 0; i < monsterSum; i++)
        {
            putObjectOnMap(monster[i]);
            vertMoveObject(monster + i);
            horizontMoveObject(monster + i);
        }

        putObjectOnMap(mario);
        putStatOnMap();
        move(0,0);
        showMap();
        lag(3000000);


    } while (key != KEY_ESC);
    

    endwin();
    return 0;
}
