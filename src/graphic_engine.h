#ifndef DEF_GRAPHIC_ENGINE_H
#define DEF_GRAPHIC_ENGINE_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "global.h"
#include "world.h"

typedef struct 
{
    int screen_w, screen_h;
    const char* window_name;

    int is_running;

    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* event_queue;

    int cell_h, cell_w;


    World* world;
} GraphicEngine;

GraphicEngine* graphic_engine_new(int screen_w, int screen_h, 
                                  const char* window_name, World* world);
void graphic_engine_die(GraphicEngine* engine);

void graphic_engine_init(GraphicEngine* engine);
void graphic_engine_run(GraphicEngine* engine);

#endif