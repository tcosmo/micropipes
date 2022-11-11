#include "world.h"

World* new_world(int height, int width)
{
    World* world = (World*) malloc(sizeof(World));
    world->height = height;
    world->width = width;

    world->cells = (Cell**) calloc(height, sizeof(Cell*));
    for( int row = 0 ; row < height ; row += 1 )
        world->cells[row] = (Cell*) calloc(width, sizeof(Cell));

    for( int row = 0 ; row < height ; row += 1 )
        for( int col = 0 ; col < width ; col += 1) {
            world->cells[row][col].bit = CELL_UNDEFINED;
            world->cells[row][col].carry = CELL_UNDEFINED;
        }

    world->cyclic_border = EMPTY_BORDER;

    return world;
}

void deduce_cyclic_border_initial_layer(World* world)
{
    int norm = border_norm(world->cyclic_border);
    for( int i = norm ; i >= 0 ; i -= 1 ) {
        point2D p = get_cell_position_on_border(world, 0, i);
        world->cells[p.row][p.col].carry = 1;
    }

    for( int i = norm ; i >= 1 ; i -= 1 ) {
        char c = world->cyclic_border.border_string[i-1];
        if( c == '1' ) {
            for( int j = i ; j <= norm ; j += 1 ) {
                point2D p = get_cell_position_on_border(world, 0, j);
                if( world->cells[p.row][p.col].bit != CELL_UNDEFINED )
                    break;
                world->cells[p.row][p.col].bit = 1-(j-i)%2;
            }
        }
    }

    point2D p = get_cell_position_on_border(world, 0, norm);
    int last_bit = world->cells[p.row][p.col].bit;

    for( int i = 0 ; i < norm ; i += 1 ) {
        p = get_cell_position_on_border(world, 0, i);
        if( world->cells[p.row][p.col].bit != CELL_UNDEFINED )
                    break;
        if(i%2)
            world->cells[p.row][p.col].bit = 1-last_bit;
        else
            world->cells[p.row][p.col].bit = last_bit;
    }
}

int valid_position(World* w, point2D p)
{
    return p.col >= 0 && p.row >= 0 && p.col < w->width && p.row < w->height;
}

int completely_defined(World* w, point2D p)
{
    Cell c = w->cells[p.row][p.col];
    return c.bit != CELL_UNDEFINED && c.carry != CELL_UNDEFINED;
}

Cell deduce_cell(World* w, point2D east, point2D south)
{
    Cell c_east =  w->cells[east.row][east.col];
    Cell c_south =  w->cells[south.row][south.col];
    Cell to_ret;
    to_ret.bit = abs(c_south.bit-(c_east.bit+c_east.carry))%2;
    to_ret.carry = to_ret.bit + c_east.bit+c_east.carry >= 2;
    return to_ret;
}

int world_deduce_once(World* world)
{
    int progress = 0;
    for( int row = 0 ; row < world->height ; row += 1 )
        for( int col = 0 ; col < world->width ; col += 1 ) {
            
            point2D me = {row, col};

            if( completely_defined(world, me) )
                continue;

            point2D east = {row,col+1};
            point2D south = {row+1, col};

            if( valid_position(world, east) && valid_position(world, south) ) {
                if( completely_defined(world, east) && completely_defined(world, south) ) {
                    world->cells[row][col] = deduce_cell(world, east, south);
                    progress = 1;
                }
            }
        }
    return progress;
}

int world_deduce_many(World* world)
{
    int progress = 0;
    while(world_deduce_once(world)) { progress = 1; }
    return progress;
}

void free_cyclic_parity_break(World* world)
{
    int norm = border_norm(world->cyclic_border);   
    if(!norm) return;

    for( int row = 0 ; row < world->height ; row += 1 )
        free(world->cyclic_parity_break[row]);
    free(world->cyclic_parity_break);
}

void set_cyclic_border(World* world, Border b, int center_border, int row, int col)
{
    //I thought that the parity break array dim depended on the border
    //but it doesnt, it just depends on the world
    //so the following free is a bit useless, but whatever.
    //the idea is that we dont need the mem if we dont have a cyclic border
    //and we want to be able to change border on the go.
    border_free(world->cyclic_border);
    free_cyclic_parity_break(world);
    world->cyclic_parity_break = (int**) calloc(world->height, sizeof(int*));
    for( int row = 0 ; row < world->height ; row += 1 )
        world->cyclic_parity_break[row] = (int*) calloc(world->width, sizeof(int));
    for( int row = 0 ; row < world->height ; row += 1)
        for(int col = 0 ; col < world->width ; col += 1 )
            world->cyclic_parity_break[row][col] = CELL_UNDEFINED;
            
    world->cyclic_border = b;
    if(!center_border) {
        world->cyclic_border_row = row;
        world->cyclic_border_col = col;
    }
    else {
        world->cyclic_border_row = world->height-1-border_span(b);
        world->cyclic_border_col = (world->width - border_norm(b))/2 + border_norm(b);
    }
}

void apply_cyclic_knwoledge(World* world)
{
    int norm = border_norm(world->cyclic_border);
    int layer = 0;
    point2D end = get_cell_position_on_border(world, layer, norm);
    while( valid_position(world, end) && completely_defined(world, end) ) {
        point2D deb = get_cell_position_on_border(world, layer, 0);
        if( !valid_position(world, deb) )
            break;
        world->cells[deb.row][deb.col] = world->cells[end.row][end.col];
        end.row -= 1;
        layer += 1;
    }
}

int cell_sum(World* world, point2D p)
{
    Cell c = world->cells[p.row][p.col];
    return c.bit+c.carry;
}

void compute_cyclic_parity_break(World* world)
{
    int norm = border_norm(world->cyclic_border);
    if(!norm) return;

    point2D p = get_cell_position_on_border(world, 0, 0);
    int layer = 0;
    while( valid_position(world, p) ) {
        for( int i = 0 ; i < norm ; i += 1 ) {
            point2D p2 = get_cell_position_on_border(world, layer, i);
            if( !valid_position(world, p2) ) continue;
            if( layer == 0 )
                world->cyclic_parity_break[p2.row][p2.col] = 
                    (cell_sum(world,p2)%2 != (world->cyclic_border.border_string[i]-'0'));
            else {
                if (cell_sum(world,p2)%2)
                    world->cyclic_parity_break[p2.row][p2.col] = 1-world->cyclic_parity_break[p2.row+1][p2.col];
                else
                    world->cyclic_parity_break[p2.row][p2.col] = world->cyclic_parity_break[p2.row+1][p2.col];
            }
        }
        point2D p2 = get_cell_position_on_border(world, layer, norm);
        world->cyclic_parity_break[p2.row][p2.col] = world->cyclic_parity_break[p.row][p.col];
        layer += 1;
        p = get_cell_position_on_border(world, layer, 0);
    }
}

void clear_world(World* world)
{
    for( int row = 0 ; row < world->height ; row += 1 )
        for( int col = 0 ; col < world->width ; col += 1 ) {
            Cell c = {CELL_UNDEFINED,CELL_UNDEFINED};
            world->cells[row][col] = c;
        }
}

void world_rotate_border(World* world, int r)
{
    int norm = border_norm(world->cyclic_border);
    if( !norm ) return;

    Border rotated = border_rotate(world->cyclic_border, r);
    set_cyclic_border(world, rotated, 1, -1, -1);
    clear_world(world);
    
    deduce_cyclic_border_initial_layer(world);
    world_deduce_many(world);
    //world_deduce_cycle(world, 0);
}

void world_deduce_cycle(World* world, int only_beginning)
{
    int norm = border_norm(world->cyclic_border);
    if( !norm )
        return;

    deduce_cyclic_border_initial_layer(world);

    if(only_beginning)
        return;

    while(world_deduce_many(world))
        apply_cyclic_knwoledge(world);

    compute_cyclic_parity_break(world);
}

void destroy_world(World* world)
{
    free_cyclic_parity_break(world);
    for( int row = 0 ; row < world->height ; row += 1)
        free(world->cells[row]);
    free(world->cells);
    free(world);
}

void world_expand_cycle(World* w)
{
    int norm = border_norm(w->cyclic_border);
    if( !norm ) return;
    int span = border_span(w->cyclic_border);
    for( int row = 0 ; row < w->height ; row += 1 )
        for(int col = 0 ; col < w->width ; col += 1 ) {
            point2D p = {row, col};
            if( !completely_defined(w, p)) continue;

            point2D p_translate = {row-span, col+norm};
            if( valid_position(w, p_translate) ) {
                w->cells[p_translate.row][p_translate.col] = w->cells[row][col];
            }
        }

}

point2D get_cell_position_on_border(World* world, int i_layer, int i_pos)
{
    int row = world->cyclic_border_row-i_layer;
    point2D p = {-1,-1};
    
    if( row < 0 )
        return p;
    
    int col = world->cyclic_border_col;

    if( i_pos == 0 ) {
        p.row = row;
        p.col = col;
        return p;
    }

    int norm = border_norm(world->cyclic_border);
    int j = 1;
    for( int i = 0 ; i < norm ; i += 1 ) {
        char c = world->cyclic_border.border_string[i];
        if( c == '1' )
            row += 1;
        col -= 1; 

        if( j == i_pos ) {
            p.row = row;
            p.col = col;
            return p;
        }

        j += 1;
    }

    return p;
}