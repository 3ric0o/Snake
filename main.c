#include <stdio.h>
#include "raylib.h"
#include "snake_list.h"
#include "snake_array.h"

enum
{
    Moving_Up,
    Moving_Down,
    Moving_Left,
    Moving_Right

};

void check_dataStructure(bool using_array, SnakeInterface **snake)
{
    *snake = using_array ? get_snake_array(false) : get_snake_list(false);
}

void get_input(int *direction)
{
    if(IsKeyPressed(KEY_W) && *direction != Moving_Down)
    {
        *direction = Moving_Up;
    }
    else if(IsKeyPressed(KEY_S) && *direction != Moving_Up)
    {
        *direction = Moving_Down;
    }
    else if(IsKeyPressed(KEY_A) && *direction != Moving_Right)
    {
        *direction = Moving_Left;
    }
    else if(IsKeyPressed(KEY_D) && *direction != Moving_Left)
    {
        *direction = Moving_Right;
    }
}

void constantly_move(SnakeInterface *snake, int direction)
{
    switch(direction)
    {
        case Moving_Up:
            snake->move(0, -30);
            break;
        case Moving_Down:
            snake->move(0, 30);
            break;
        case Moving_Left:
            snake->move(-30, 0);
            break;
        case Moving_Right:
            snake->move(30, 0);
            break;
    }
}

int main(void)
{
    SnakeInterface *snake = NULL;
    bool using_array = true;
    int direction = Moving_Up;

    InitWindow(1600,900,"Snake Game");
    SetTargetFPS(15);

    //check_dataStructure(using_array, &snake);
    snake = get_snake_array(true);
    snake->init(800,450);

    while (!WindowShouldClose())
    {
        /*if (IsKeyPressed(KEY_SPACE))
        {
            using_array = !using_array;
            check_dataStructure(using_array, &snake);
        }*/

        BeginDrawing();
        ClearBackground(BLACK);
        snake->draw();

        get_input(&direction);
        constantly_move(snake, direction);

        EndDrawing();
    }

    if (snake != NULL)
    {
        snake->cleanup();
    }
    CloseWindow();
    return 0;
}
