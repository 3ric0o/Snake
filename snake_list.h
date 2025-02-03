#ifndef SNAKE_SNAKE_LIST_H
#define SNAKE_SNAKE_LIST_H

#include "snake_interface.h"
#include "snake_common.h"

//Get the list implementation of the snake
SnakeInterface *get_snake_list(bool first_init);

void list_cleanup(void);

#endif //SNAKE_SNAKE_LIST_H
