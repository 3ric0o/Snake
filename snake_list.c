#include "snake_interface.h"
#include "snake_array.h"
#include <stdlib.h>
#include <raylib.h>
#include <stdio.h>

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
static void list_init(int x, int y)
{
    printf("DEBUG: Initializing snake at (%d, %d)\n", x, y);

    // Clear any existing nodes
    while (snake_list.head != NULL)
    {
        SnakeNode *temp = snake_list.head;
        snake_list.head = snake_list.head->next;
        free(temp);
    }

    // Create initial node
    snake_list.head = malloc(sizeof(SnakeNode));
    if (snake_list.head == NULL) {
        printf("DEBUG: Failed to allocate initial node\n");
        return;
    }

    // Ensure starting position is grid-aligned
    x = (x / 30) * 30;
    y = (y / 30) * 30;

    snake_list.head->pos.x = x;
    snake_list.head->pos.y = y;
    snake_list.head->next = NULL;
    snake_list.length = 1;

    printf("DEBUG: Snake initialized. Length: %d\n", snake_list.length);
}

static void list_move(int x, int y)
{
    if (snake_list.head == NULL) {
        printf("DEBUG: Error - Head is NULL\n");
        return;
    }

    // Single node case
    if (snake_list.length == 1) {
        snake_list.head->pos.x += x;
        snake_list.head->pos.y += y;
        printf("DEBUG: Single node moved to (%d, %d)\n",
               snake_list.head->pos.x, snake_list.head->pos.y);
        return;
    }

    // For multiple nodes:
    // 1. Create new head
    SnakeNode* new_head = malloc(sizeof(SnakeNode));
    if (!new_head) {
        printf("DEBUG: Failed to allocate new head\n");
        return;
    }

    // 2. Set new head position
    new_head->pos.x = snake_list.head->pos.x + x;
    new_head->pos.y = snake_list.head->pos.y + y;

    // 3. Insert new head
    new_head->next = snake_list.head;
    snake_list.head = new_head;

    // 4. Remove tail (unless growing)
    SnakeNode* current = snake_list.head;
    while (current->next != NULL && current->next->next != NULL) {
        current = current->next;
    }

    if (current->next != NULL) {
        free(current->next);
        current->next = NULL;
    }
}

static void list_grow(void)
{
    if (snake_list.length >= MAX_SNAKE_LENGTH) {
        return;
    }

    // For single node, duplicate it
    if (snake_list.length == 1) {
        SnakeNode* new_node = malloc(sizeof(SnakeNode));
        if (!new_node) {
            puts("Failed to allocate first node on snake.");
            return;
        }
        new_node->pos = snake_list.head->pos;
        new_node->next = NULL;
        snake_list.head->next = new_node;
        snake_list.length = 2;
        return;
    }

    // For multiple nodes, copy the last node
    SnakeNode* current = snake_list.head;
    while (current->next != NULL) {
        current = current->next;
    }

    SnakeNode* new_node = malloc(sizeof(SnakeNode));
    if (!new_node) {
        printf("Failed to allocated %d node of snake", snake_list.length);
        return;
    }
    new_node->pos = current->pos;
    new_node->next = NULL;
    current->next = new_node;
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
    if (snake_list.head == NULL) {
        printf("DEBUG: Head is NULL in get_head\n");
        return (Position){0, 0};
    }
    return snake_list.head->pos;
}

static int list_get_length(void)
{
    return snake_list.length;
}

static void list_draw(void)
{
    if (snake_list.head == NULL) {
        printf("DEBUG: Head is NULL in draw\n");
        return;
    }

    SnakeNode *current = snake_list.head;
    int segments = 0;

    while (current != NULL && segments < snake_list.length)
    {
        DrawRectangle(current->pos.x, current->pos.y, 30, 30, BLUE);
        printf("DEBUG: Drawing segment %d at (%d, %d)\n",
               segments, current->pos.x, current->pos.y);
        current = current->next;
        segments++;
    }

    printf("DEBUG: Drew %d segments\n", segments);
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
    static Position positions[MAX_SNAKE_LENGTH];
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
        snake_list.head = NULL;
        snake_list.length = 0;

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
        snake_interface.get_positions = list_get_positions;
    }

    return &snake_interface;
}