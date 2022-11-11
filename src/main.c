
#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "world.h"
#include "border.h"
#include "graphic_engine.h"

//Border cyclic_border = { "001110101000" };
//Nice: Border cyclic_border = { "0010" };
//Nice plein de 000: Border cyclic_border = { "001110101000" };
//The cycle: Border cyclic_border = { "010101" };
//Border cyclic_border = { "010101" };
//Nice plein de 0 mieux; Border cyclic_border = { "000111010100" };

//Border cyclic_border = { "1010110001010100101110101000000", 0};

Border cyclic_border = { "1110111" };

//Border cyclic_border = { "00000000001111111000000000111111111" };


//Border cyclic_border = { "10001001101100001011" };


//Border cyclic_border = { "010101" };

int main (int argc, char *argv[])
{
    World* world = new_world(100,100);
    set_cyclic_border(world, cyclic_border, 0, world->height-border_span(cyclic_border)-1, border_norm(cyclic_border));
    
    world_deduce_cycle(world, 1);

    GraphicEngine* graphic_engine = graphic_engine_new(1000, 800, "Micropipes", 
                                                       world);
    
    graphic_engine_init(graphic_engine);
    graphic_engine_run(graphic_engine);
    graphic_engine_die(graphic_engine);

    destroy_world(world);
    
    return 0;
}