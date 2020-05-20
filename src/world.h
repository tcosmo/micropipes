#ifndef DEF_WORLD_H
#define DEF_WORLD_H

#include "global.h"
#include "border.h"

typedef struct
{
    int bit;
    int carry;
} Cell;

typedef struct
{
    int height, width;
    Cell** cells;

    Border cyclic_border;
    int cyclic_border_row, cyclic_border_col;
} World;

World* new_world(int height, int width);
void destroy_world(World* world);

void set_cyclic_border(World* world, Border b, int center_border, int row, int col);

#endif