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

    engine->show_cell_content = 1;
    engine->show_parity_break = 0;

    return engine;
}

void graphic_engine_die(GraphicEngine* engine)
{
    al_destroy_display(engine->display);
    al_destroy_event_queue(engine->event_queue);
    al_destroy_font(engine->font_arial72);
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
    assert(al_init_font_addon());
    assert(al_init_ttf_addon());
    engine->font_arial72 = al_load_font("arial.ttf",engine->cell_w-5,0);
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

float get_scaled_thickness(GraphicEngine* engine)
{
    return (1/engine->scaling_factor);
}

void draw_grid(GraphicEngine* engine)
{
    for (int col = 0; col <= engine->world->width; col += 1) {
        al_draw_line(col * engine->cell_w, 0, col * engine->cell_w,
                     engine->world->height * engine->cell_h, GRID_COLOR, 1*get_scaled_thickness(engine));
    }

    for (int row = 0; row <= engine->world->height; row += 1) {
        al_draw_line(0, row * engine->cell_h, engine->world->width * engine->cell_w,
                     row * engine->cell_h, GRID_COLOR, 1*get_scaled_thickness(engine));
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
    engine->camera_x += ((float) dx)/(engine->scaling_factor);
    engine->camera_y += ((float) dy)/(engine->scaling_factor);
}

void camera_set_position(GraphicEngine* engine, int x, int y)
{
    camera_translate(engine, (-x-engine->camera_x)*(engine->scaling_factor), (-y-engine->camera_y)*(engine->scaling_factor));
}

void camera_translate_on_border(GraphicEngine* engine)
{   
    int norm = border_norm(engine->world->cyclic_border);
    if( norm ) {
        camera_set_position(engine, (engine->world->cyclic_border_col-norm-1)*engine->cell_w, 
                            (engine->world->height-10)*engine->cell_h);
    }
}

void camera_zoom(GraphicEngine* engine, float zoom_factor)
{
    engine->scaling_factor *= zoom_factor;
    al_scale_transform(&engine->camera_transform, zoom_factor, zoom_factor);
    al_use_transform(&engine->camera_transform);
}

void fill_cell(GraphicEngine* engine, int col, int row, ALLEGRO_COLOR c)
{
    al_draw_filled_rectangle(col*engine->cell_w,row*engine->cell_h,
                             (col+1)*engine->cell_w,(row+1)*engine->cell_h,c);
}


void render_cell(GraphicEngine* engine, int row, int col, Cell c)
{
    // if( c.bit == 1 && c.carry == 1 )
    //     al_draw_filled_rectangle(col*engine->cell_w, row*engine->cell_h, 
    //                              (col+1)*engine->cell_w, (row+1)*engine->cell_h, WHITE);


    if(engine->kustom_coloring) {
        if( c.bit == CELL_UNDEFINED || c.carry == CELL_UNDEFINED )
            return;
        if( c.bit == 0 && c.carry == 0  )
            fill_cell(engine, col, row, GREEN);
        else if(c.bit == 1 && c.carry == 0)
            fill_cell(engine, col, row, PINK);

        if( c.bit == 1 && c.carry == 1 )
            fill_cell(engine, col, row, BLUE);
        if( c.bit == 0 && c.carry == 1 )
            fill_cell(engine, col, row, BLACK);
    }

    if(engine->show_cell_content) {
        if( c.bit != CELL_UNDEFINED ) {
            char text_bit[2];
            sprintf(text_bit, "%d", c.bit);
            int text_w = al_get_text_width(engine->font_arial72, text_bit);
            int text_height = al_get_font_line_height(engine->font_arial72);
            al_draw_text(engine->font_arial72, WHITE, col*engine->cell_w+(engine->cell_w-text_w)/2, 
                        row*engine->cell_h+(engine->cell_h-text_height), 0, text_bit);

        }

        if( c.carry != CELL_UNDEFINED ) {
            if( c.carry == 1 ) {
                al_draw_line(col*engine->cell_w+5, row*engine->cell_h+4,
                            (col+1)*engine->cell_w-5, row*engine->cell_h+4, WHITE, 3*get_scaled_thickness(engine));
            }
        }
    }
}

void render_cells(GraphicEngine* engine)
{
    World* w = engine->world;
    for( int row = 0 ; row < w->height ; row += 1)
        for( int col = 0 ; col < w->width ; col += 1 )
            render_cell(engine, row, col, w->cells[row][col]);
}

void set_cell_border(GraphicEngine* engine, int col, int row, int dir, 
                     ALLEGRO_COLOR color, int thickness)
{
    switch(dir) {
        case SOUTH:
            al_draw_line(col*engine->cell_w, (row+1)*engine->cell_h,
                         (col+1)*engine->cell_w, (row+1)*engine->cell_h,
                         color, thickness*get_scaled_thickness(engine));
            break;
        case EAST:
            al_draw_line((col+1)*engine->cell_w, (row)*engine->cell_h,
                         (col+1)*engine->cell_w, (row+1)*engine->cell_h,
                         color, thickness*get_scaled_thickness(engine));
            break;
        case WEST:
            al_draw_line((col)*engine->cell_w, (row)*engine->cell_h,
                         (col)*engine->cell_w, (row+1)*engine->cell_h,
                         color, thickness*get_scaled_thickness(engine));
            break;
        default:
            return;
    }
}

void render_border(GraphicEngine* engine)
{
    int norm = border_norm(engine->world->cyclic_border); 
    if( norm ) {
        int span = border_span(engine->world->cyclic_border);

        ALLEGRO_COLOR border_color = (pow(2,norm) > pow(3, span)) ? GREEN : RED;

        int col = engine->world->cyclic_border_col;
        int row = engine->world->cyclic_border_row;
        
        set_cell_border(engine, col, row, SOUTH, border_color, DEFAULT_CELL_BORDER_THICKNESS);
        col -= 1;

        for( int i = 0 ; i < norm ; i += 1 ) {
            char bit = engine->world->cyclic_border.border_string[i];
            if( bit == '1' ) {
                row += 1;
                set_cell_border(engine, col, row, EAST, border_color, DEFAULT_CELL_BORDER_THICKNESS);
            }
            set_cell_border(engine, col, row, SOUTH, border_color, DEFAULT_CELL_BORDER_THICKNESS);
            col -= 1;
        }
    }
}

void render_cell_parity_break(GraphicEngine* engine, point2D p)
{
    if(!valid_position(engine->world, p)) return;

    int parity_break = engine->world->cyclic_parity_break[p.row][p.col];
    if( parity_break == CELL_UNDEFINED ) return;

    ALLEGRO_COLOR c = (parity_break) ? BLACK : BACKGROUND_COLOR;
    fill_cell(engine, p.col, p.row, c);
}

void render_parity_break(GraphicEngine* engine)
{
    int norm = border_norm(engine->world->cyclic_border);
    if(!norm) return;

    point2D p = get_cell_position_on_border(engine->world, 0, 0);
    int layer = 0;
    while( valid_position(engine->world, p) ) {
        for( int i = 0 ; i <= norm ; i += 1 ) {
            point2D p2 = get_cell_position_on_border(engine->world, layer, i);
            if( !valid_position(engine->world, p2) ) continue;
            render_cell_parity_break(engine, p2);
        }
        layer += 1;
        p = get_cell_position_on_border(engine->world, layer, 0);
    }
}

void render_flat_world(GraphicEngine* engine)
{
    int norm = border_norm(engine->world->cyclic_border);
    if(!norm) return;

    
    int layer = 0;
    int col = engine->world->cyclic_border_col;
    int row = engine->world->height-1;
    int ok = 1;
    while(ok) {
        ok = 0;
        int s = 0;
        int s2 = 0;
        int s3 = 0;
        for( int i = 0 ; i <= norm ; i += 1 ) {
            point2D p = get_cell_position_on_border(engine->world, layer, norm-i);

            if(valid_position(engine->world,p)) {
                ok = 1;
                Cell c = engine->world->cells[p.row][p.col];
                if(i%2 == 0)
                s += c.bit;
                else
                    s2 += c.bit;
                //s3 += c.carry+c.bit; 
                //render_cell(engine, row-layer, col+i, c);
            }
        }
        
        Cell c = {(s%3)>=1,(s%3)==2};
        render_cell(engine, row-layer, col+norm+1, c);
        Cell c2 = {(s2%3)>=1,(s2%3)==2};
        render_cell(engine, row-layer, col+norm+2, c2);
        // Cell c3 = {s3%2,(s3%3)%2};
        // render_cell(engine, row-layer, col+norm+3, c3);
        layer+= 1;
    }
}

void graphic_engine_run(GraphicEngine* engine)
{
    camera_translate_on_border(engine);
    camera_zoom(engine, DEFAULT_ZOOM_FACTOR);

    while(engine->is_running) {
        al_clear_to_color(BACKGROUND_COLOR);

        if(engine->show_grid)
            draw_grid(engine);

        if(engine->show_parity_break)
            render_parity_break(engine);

        
        render_cells(engine);
        render_border(engine);
        //render_flat_world(engine);
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
                case ALLEGRO_KEY_C: 
                    camera_translate_on_border(engine);
                    break;
                case ALLEGRO_KEY_D:
                    world_deduce_many(engine->world); 
                    break;
                case ALLEGRO_KEY_P:
                    engine->show_parity_break = 1-engine->show_parity_break;
                    break;
                case ALLEGRO_KEY_H:
                    engine->show_cell_content = 1-engine->show_cell_content;
                    break;
                case ALLEGRO_KEY_K:
                    engine->kustom_coloring = 1-engine->kustom_coloring;
                    break;
                case ALLEGRO_KEY_M:
                    world_rotate_border(engine->world, 1);
                    printf("%s\n", engine->world->cyclic_border.border_string);
                    break;
                case ALLEGRO_KEY_L:
                    world_rotate_border(engine->world, -1);
                    printf("%s\n", engine->world->cyclic_border.border_string);
                    break;
                case ALLEGRO_KEY_S:
                    world_expand_cycle(engine->world);
                    break;
                case ALLEGRO_KEY_Y:
                    world_deduce_once(engine->world);
                    break;
                case ALLEGRO_KEY_U:
                    world_deduce_many(engine->world);
                    break;
                case ALLEGRO_KEY_I:
                    apply_cyclic_knwoledge(engine->world);
                    break;
                case ALLEGRO_KEY_O:
                    while(world_deduce_many(engine->world))
                    apply_cyclic_knwoledge(engine->world);
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