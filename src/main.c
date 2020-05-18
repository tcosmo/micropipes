
#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "world.h"
#include "graphic_engine.h"

int main (int argc, char *argv[])
{
    World* world = new_world(1000,1000);

    GraphicEngine* graphic_engine = graphic_engine_new(1000, 800, "Micropipes", 
                                                       world);
    graphic_engine_init(graphic_engine);
    graphic_engine_run(graphic_engine);
    graphic_engine_die(graphic_engine);

    destroy_world(world);
    
    return 0;
}