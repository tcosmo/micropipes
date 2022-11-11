#ifndef DEF_BORDER_H
#define DEF_BORDER_H

#include "global.h"

typedef struct 
{
    char* border_string;
    int to_free;
} Border;

Border new_border(const char* border_string);
unsigned int border_norm(Border b);
unsigned int border_span(Border b);

Border border_rotate(Border b, int r);
void border_free(Border b);

extern const Border EMPTY_BORDER;

#endif