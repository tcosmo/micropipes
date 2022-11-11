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
    int row;
    int col;
} point2D;

typedef struct
{
    int height, width;
    Cell** cells;

    Border cyclic_border;
    int cyclic_border_row, cyclic_border_col;
    int** cyclic_parity_break;
} World;

World* new_world(int height, int width);
void destroy_world(World* world);

void set_cyclic_border(World* world, Border b, int center_border, int row, int col);

point2D get_cell_position_on_border(World* world, int i_layer, int i_pos);

int world_deduce_once(World* world);
int world_deduce_many(World* world);
void world_deduce_cycle(World* world, int only_beginning);
int valid_position(World* world, point2D p);
void world_rotate_border(World* world, int r);
void world_expand_cycle(World* world);
void apply_cyclic_knwoledge(World* world);

#endif