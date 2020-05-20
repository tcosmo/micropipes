#ifndef DEF_BORDER_H
#define DEF_BORDER_H

#include "global.h"

typedef struct 
{
    const char* border_string;
} Border;

Border new_border(const char* border_string);
unsigned int border_norm(Border b);
unsigned int border_span(Border b);

#endif