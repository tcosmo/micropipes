#ifndef DEF_GLOBAL_H
#define DEF_GLOBAL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define WHITE al_map_rgb_f(1,1,1)
#define BLACK al_map_rgb_f(0,0,0)
#define BACKGROUND_COLOR al_map_rgb_f(0.3, 0.3, 0.3)
#define GRID_COLOR al_map_rgb_f(0.5, 0.5, 0.5)
#define GREEN al_map_rgb(50,255,50)
#define PINK al_map_rgb(255,44,180)
#define BLUE al_map_rgb(30,44,230)

#define CELL_UNDEFINED -1

#define DEFAULT_CELL_DIM_PX 20
#define DEFAULT_CAMERA_TRANSLATE 10
#define DEFAULT_ZOOM_FACTOR 2.0
#define DEFAULT_CELL_BORDER_THICKNESS 3.0

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#endif