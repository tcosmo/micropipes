#include "graphic_engine.h"

GraphicEngine* graphic_engine_new(int screen_w, int screen_h,
                                  const char* window_name, World* world)
{
    GraphicEngine* engine = (GraphicEngine*)calloc(1, sizeof(GraphicEngine));
    engine->screen_w = screen_w;
    engine->screen_h = screen_h;
    engine->window_name = window_name;
    engine->is_running = 1;
    engine->world = world;

    engine->scaling_factor = 1.0;

    engine->cell_w = DEFAULT_CELL_DIM_PX;
    engine->cell_h = DEFAULT_CELL_DIM_PX;

    return engine;
}

void graphic_engine_die(GraphicEngine* engine)
{
    al_destroy_display(engine->display);
    al_destroy_event_queue(engine->event_queue);
    free(engine);
}

void graphic_engine_init(GraphicEngine* engine)
{
    assert(al_init());
    engine->display = al_create_display(engine->screen_w,
                                        engine->screen_h);
    assert(engine->display);
    assert(al_init_image_addon());
    assert(al_init_primitives_addon());
    al_set_window_title(engine->display, engine->window_name);
    assert(al_install_keyboard());
    assert(al_install_mouse());
    engine->event_queue = al_create_event_queue();
    assert(engine->event_queue);
    al_register_event_source(engine->event_queue,
                             al_get_display_event_source(engine->display));
    al_register_event_source(engine->event_queue,
                             al_get_keyboard_event_source());
    al_register_event_source(engine->event_queue,
                             al_get_mouse_event_source());

    al_identity_transform(&engine->camera_transform);
}

void draw_grid(GraphicEngine* engine)
{
    for (int col = 0; col <= engine->world->width; col += 1) {
        al_draw_line(col * engine->cell_w, 0, col * engine->cell_w,
                     engine->world->height * engine->cell_h, GRID_COLOR, 1);
    }

    for (int row = 0; row <= engine->world->height; row += 1) {
        al_draw_line(0, row * engine->cell_h, engine->world->width * engine->cell_w,
                     row * engine->cell_h, GRID_COLOR, 1);
    }
}

int mode_camera()
{
    ALLEGRO_KEYBOARD_STATE keyState;
	al_get_keyboard_state(&keyState);
	return al_key_down(&keyState, ALLEGRO_KEY_LCTRL);
}

void camera_translate(GraphicEngine* engine, int dx, int dy)
{
    al_translate_transform(&engine->camera_transform, dx, dy);
    al_use_transform(&engine->camera_transform);
}

void camera_zoom(GraphicEngine* engine, float zoom_factor)
{
    engine->scaling_factor *= zoom_factor;
    al_scale_transform(&engine->camera_transform, zoom_factor, zoom_factor);
    al_use_transform(&engine->camera_transform);
}

void graphic_engine_run(GraphicEngine* engine)
{

    while(engine->is_running) {
        al_clear_to_color(BACKGROUND_COLOR);
        if (engine->show_grid)
            draw_grid(engine);

        al_draw_filled_rectangle(0,0,20,20,BLACK);
        al_flip_display();

        ALLEGRO_EVENT event;
        al_wait_for_event(engine->event_queue, &event);

        if( event.type == ALLEGRO_EVENT_KEY_UP ) {
            switch (event.keyboard.keycode) {
            }
        }

        if( event.type == ALLEGRO_EVENT_KEY_DOWN ) {

            if( mode_camera(engine) ) {
                switch (event.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                        camera_translate(engine, 0, DEFAULT_CAMERA_TRANSLATE*engine->cell_h);
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        camera_translate(engine, -DEFAULT_CAMERA_TRANSLATE*engine->cell_w, 0);
                        break;
                    case ALLEGRO_KEY_DOWN:
                        camera_translate(engine, 0, -DEFAULT_CAMERA_TRANSLATE*engine->cell_h);
                        break;
                    case ALLEGRO_KEY_LEFT:
                        camera_translate(engine, DEFAULT_CAMERA_TRANSLATE*engine->cell_w, 0);
                        break;

                    case ALLEGRO_KEY_Z:
                        camera_zoom(engine, DEFAULT_ZOOM_FACTOR);
                        break;

                    case ALLEGRO_KEY_E:
                        camera_zoom(engine, 1/DEFAULT_ZOOM_FACTOR);
                        break;
                }
            }

            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    break;
                case ALLEGRO_KEY_RIGHT:
                    break;
                case ALLEGRO_KEY_DOWN:
                    break;
                case ALLEGRO_KEY_LEFT:
                    break;
                case ALLEGRO_KEY_G:
                    engine->show_grid = 1 - engine->show_grid;
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    engine->is_running = 0;
                    break;
            }
        }

        if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE )
            engine->is_running = 0;
    }
}