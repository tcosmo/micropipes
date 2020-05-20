#include "world.h"

World* new_world(int height, int width)
{
    World* world = (World*) malloc(sizeof(World));
    world->height = height;
    world->width = width;

    world->cells = (Cell**) calloc(height, sizeof(Cell*));
    for( int row = 0 ; row < height ; row += 1 )
        world->cells[row] = (Cell*) calloc(width, sizeof(Cell));

    return world;
}

void set_cyclic_border(World* world, Border b, int center_border, int row, int col)
{
    world->cyclic_border = b;
    if(!center_border) {
        world->cyclic_border_row = row;
        world->cyclic_border_col = col;
    }
    else {
        world->cyclic_border_row = world->height-1-border_span(b);
        world->cyclic_border_col = (world->width - border_norm(b))/2 + border_norm(b);
    }
    Cell c = {1,1};
    world->cells[world->cyclic_border_row][world->cyclic_border_col] = c;
}

void destroy_world(World* world)
{
    for( int row = 0 ; row < world->height ; row += 1)
        free(world->cells[row]);
    free(world->cells);
    free(world);
}