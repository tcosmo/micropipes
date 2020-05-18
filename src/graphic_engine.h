#ifndef DEF_GRAPHIC_ENGINE_H
#define DEF_GRAPHIC_ENGINE_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "global.h"
#include "world.h"

typedef struct 
{
    int screen_w, screen_h;
    const char* window_name;

    float scaling_factor;

    int is_running;

    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* event_queue;
    ALLEGRO_MOUSE_STATE mouse_state;

    ALLEGRO_TRANSFORM camera_transform;

    int cell_h, cell_w;

    int show_grid;

    World* world;
} GraphicEngine;

GraphicEngine* graphic_engine_new(int screen_w, int screen_h, 
                                  const char* window_name, World* world);
void graphic_engine_die(GraphicEngine* engine);

void graphic_engine_init(GraphicEngine* engine);
void graphic_engine_run(GraphicEngine* engine);

#endif