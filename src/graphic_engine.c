#include "graphic_engine.h"

GraphicEngine *graphic_engine_new(int screen_w, int screen_h,
                                  const char *window_name, World *world)
{
    GraphicEngine *engine = (GraphicEngine *)malloc(sizeof(GraphicEngine));
    engine->screen_w = screen_w;
    engine->screen_h = screen_h;
    engine->window_name = window_name;
    engine->is_running = 1;
    engine->world = world;

    engine->cell_w = DEFAULT_CELL_DIM_PX;
    engine->cell_h = DEFAULT_CELL_DIM_PX;

    return engine;
}

void graphic_engine_die(GraphicEngine *engine)
{
    al_destroy_display(engine->display);
    al_destroy_event_queue(engine->event_queue);
    free(engine);
}

void graphic_engine_init(GraphicEngine *engine)
{
    assert(al_init());
    assert(al_init_image_addon());
    engine->display = al_create_display(engine->screen_w,
                                        engine->screen_h);
    assert(engine->display);
    al_set_window_title(engine->display, engine->window_name);
    assert(al_install_keyboard());
    engine->event_queue = al_create_event_queue();
    assert(engine->event_queue);
    al_register_event_source(engine->event_queue,
                             al_get_display_event_source(engine->display));
    al_register_event_source(engine->event_queue,
                             al_get_keyboard_event_source());
}

void graphic_engine_run(GraphicEngine *engine)
{
    while (engine->is_running)
    {
        al_clear_to_color(BLACK);
        al_flip_display();

        ALLEGRO_EVENT event;
        al_wait_for_event(engine->event_queue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {

            switch (event.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                break;
            case ALLEGRO_KEY_RIGHT:
                break;
            case ALLEGRO_KEY_DOWN:
                break;
            case ALLEGRO_KEY_LEFT:
                break;
            case ALLEGRO_KEY_ESCAPE:
                engine->is_running = 0;
                break;
            }
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            engine->is_running = 0;
    }
}