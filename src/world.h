#ifndef DEF_WORLD_H
#define DEF_WORLD_H

#include "global.h"

typedef struct
{
    int bit;
    int carry;
} Cell;

typedef struct
{
    int height, width;
    Cell** cells;
} World;

World* new_world(int height, int width);
void destroy_world(World* world);

#endif