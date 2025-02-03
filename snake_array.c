#include "snake_interface.h"
#include "snake_common.h"
#include "snake_array.h"
#include <raylib.h>
#include <stddef.h>

#define MAX_SNAKE_LENGTH 100



static SnakeArray snake_array;
static SnakeInterface snake_interface;

// Declare external access to list cleanup
extern void list_cleanup(void);

// Implementations of the SnakeInterface functions
static void array_init(int initial_x, int initial_y)
{
    snake_array.length = 10;
    snake_array.positions[0].x = initial_x;
    snake_array.positions[0].y = initial_y;
}

static void array_move(int x, int y)
{
    for(int i = snake_array.length - 1; i > 0; i--)
    {
        snake_array.positions[i] = snake_array.positions[i - 1];
    }
    snake_array.positions[0].x += x;
    snake_array.positions[0].y += y;
}

static void array_grow(void)
{
    if(snake_array.length < MAX_SNAKE_LENGTH)
    {
        snake_array.length++;
        snake_array.positions[snake_array.length - 1] = snake_array.positions[snake_array.length - 2];
    }
}

static bool array_check_collision(Position pos)
{
    for(int i = 0; i < snake_array.length; i++)
    {
        if(snake_array.positions[i].x == pos.x && snake_array.positions[i].y == pos.y)
        {
            return true;
        }
    }
    return false;
}

static Position array_get_head(void)
{
    return snake_array.positions[0];
}

static int array_get_length(void)
{
    return snake_array.length;
}

static void array_draw(void)
{
    for(int i = 0; i < snake_array.length; i++)
    {
        DrawRectangle(snake_array.positions[i].x, snake_array.positions[i].y, 30, 30, RED);
    }
}

void array_cleanup(void)
{
    // Nothing to clean up for array implementation
}

SnakeArray* get_array_data(void)
{
    return &snake_array;
}

// Initialize the snake interface
SnakeInterface *get_snake_array(bool first_init)
{
    if(first_init)
    {
        snake_interface.init = array_init;
        snake_interface.move = array_move;
        snake_interface.grow = array_grow;
        snake_interface.check_collision = array_check_collision;
        snake_interface.get_head = array_get_head;
        snake_interface.get_length = array_get_length;
        snake_interface.draw = array_draw;
        snake_interface.cleanup = array_cleanup;
    }
    else
    {
        // Count and copy nodes from list to array
        int count = 0;
        SnakeNode *current = snake_list.head;
        while(current != NULL)
        {
            if(count < MAX_SNAKE_LENGTH)
            {
                snake_array.positions[count] = current->pos;
                count++;
            }
            current = current->next;
        }

        snake_array.length = count;
        list_cleanup();  // Clean up the old list

        // Set up the interface functions
        snake_interface.init = array_init;
        snake_interface.move = array_move;
        snake_interface.grow = array_grow;
        snake_interface.check_collision = array_check_collision;
        snake_interface.get_head = array_get_head;
        snake_interface.get_length = array_get_length;
        snake_interface.draw = array_draw;
        snake_interface.cleanup = array_cleanup;
    }

    return &snake_interface;
}