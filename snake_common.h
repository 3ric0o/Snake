#ifndef SNAKE_COMMON_H
#define SNAKE_COMMON_H

#include "snake_interface.h"

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

// Declare snake_list as external
extern SnakeList snake_list;

#endif // SNAKE_COMMON_H