#include "snake_interface.h"
#include "snake_array.h"
#include <stdlib.h>
#include <raylib.h>

#define MAX_SNAKE_LENGTH 100

typedef struct SnakeNode
{
    Position pos;
    struct SnakeNode *next;
} SnakeNode;

typedef struct
{
    SnakeNode *head;
    int length;
} SnakeList;

SnakeList snake_list;
static SnakeInterface snake_interface;

// Function declarations
extern void array_cleanup(void);

// Implementations of the SnakeInterface functions
static void list_init(int initial_x, int initial_y)
{
    snake_list.head = malloc(sizeof(SnakeNode));
    snake_list.head->pos.x = initial_x;
    snake_list.head->pos.y = initial_y;
    snake_list.head->next = NULL;
    snake_list.length = 1;
}

static void list_move(int x, int y)
{
    // Handle the case where there's only one node
    if (snake_list.length == 1)
    {
        snake_list.head->pos.x += x;
        snake_list.head->pos.y += y;
        return;
    }

    SnakeNode *current = snake_list.head;
    SnakeNode *prev = NULL;

    // Find the last node
    while(current->next != NULL)
    {
        prev = current;
        current = current->next;
    }

    // Only reposition nodes if we have more than one
    if (prev != NULL) {
        current->pos = snake_list.head->pos;
        current->next = snake_list.head;
        snake_list.head = current;
        prev->next = NULL;
        snake_list.head->pos.x += x;
        snake_list.head->pos.y += y;
    }
}

static void list_grow(void)
{
    SnakeNode *new_node = malloc(sizeof(SnakeNode));
    new_node->pos = snake_list.head->pos;
    new_node->next = snake_list.head;
    snake_list.head = new_node;
    snake_list.length++;
}

static bool list_check_collision(Position pos)
{
    SnakeNode *current = snake_list.head;
    while(current != NULL)
    {
        if(current->pos.x == pos.x && current->pos.y == pos.y)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

static Position list_get_head(void)
{
    return snake_list.head->pos;
}

static int list_get_length(void)
{
    return snake_list.length;
}

static void list_draw(void)
{
    SnakeNode *current = snake_list.head;
    while(current != NULL)
    {
        DrawRectangle(current->pos.x, current->pos.y, 30, 30, BLUE);
        current = current->next;
    }
}

void list_cleanup(void)
{
    SnakeNode *current = snake_list.head;
    while(current != NULL)
    {
        SnakeNode *next = current->next;
        free(current);
        current = next;
    }
    snake_list.head = NULL;
}

static Position* list_get_positions(void)
{
    static Position positions[MAX_SNAKE_LENGTH];  // Make sure MAX_LENGTH is defined
    SnakeNode* current = snake_list.head;
    int i = 0;

    while (current != NULL && i < MAX_SNAKE_LENGTH)
    {
        positions[i] = current->pos;
        current = current->next;
        i++;
    }

    return positions;
}

// Initialize the snake interface
SnakeInterface *get_snake_list(bool first_init)
{
    if(first_init)
    {
        snake_interface.init = list_init;
        snake_interface.move = list_move;
        snake_interface.grow = list_grow;
        snake_interface.check_collision = list_check_collision;
        snake_interface.get_head = list_get_head;
        snake_interface.get_length = list_get_length;
        snake_interface.draw = list_draw;
        snake_interface.cleanup = list_cleanup;
        snake_interface.get_positions = list_get_positions;
    }
    else
    {
        // Get array data
        SnakeArray *array = get_array_data();
        if(array == NULL)
        {
            return NULL;
        }

        // Initialize the list
        snake_list.head = NULL;
        snake_list.length = 0;

        // Copy positions from array to list in the correct order
        SnakeNode *last = NULL;
        for(int i = 0; i < array->length; i++)
        {
            SnakeNode *new_node = malloc(sizeof(SnakeNode));
            new_node->pos = array->positions[i];
            new_node->next = NULL;

            if (snake_list.head == NULL) {
                snake_list.head = new_node;
            } else {
                last->next = new_node;
            }
            last = new_node;
            snake_list.length++;
        }

        array_cleanup();

        // Set up the interface functions
        snake_interface.init = list_init;
        snake_interface.move = list_move;
        snake_interface.grow = list_grow;
        snake_interface.check_collision = list_check_collision;
        snake_interface.get_head = list_get_head;
        snake_interface.get_length = list_get_length;
        snake_interface.draw = list_draw;
        snake_interface.cleanup = list_cleanup;
    }

    return &snake_interface;
}