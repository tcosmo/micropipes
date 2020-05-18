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

void destroy_world(World* world)
{
    for( int row = 0 ; row < world->height ; row += 1)
        free(world->cells[row]);
    free(world->cells);
    free(world);
}