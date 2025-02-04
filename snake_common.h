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


extern SnakeList snake_list; // extern keyword is used to declare a variable that is defined in another file!!!!

#endif // SNAKE_COMMON_H