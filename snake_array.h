#ifndef SNAKE_SNAKE_ARRAY_H
#define SNAKE_SNAKE_ARRAY_H
#define MAX_SNAKE_LENGTH 100

#include "snake_interface.h"


SnakeInterface *get_snake_array(bool first_init);

void array_cleanup(void);

typedef struct
{
    Position positions[MAX_SNAKE_LENGTH];
    int length;
} SnakeArray;

SnakeArray* get_array_data(void);

#endif //SNAKE_SNAKE_ARRAY_H