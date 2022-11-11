#include "border.h"

const Border EMPTY_BORDER = {"",0};

unsigned border_norm(Border b)
{
    return strlen(b.border_string);
}

unsigned border_span(Border b)
{
    int nb_1 = 0;
    for( int i = 0 ; i < border_norm(b) ; i += 1 )
        nb_1 += b.border_string[i] == '1';
    return nb_1;
}

void border_free(Border b)
{
    if(b.to_free) free(b.border_string);
}

Border border_rotate(Border b, int r)
{
    int norm = border_norm(b);
    char* new_border_string = (char*)calloc(norm, sizeof(char));//LEAK
    
    for( int i = 0 ; i < norm ; i += 1 )
        new_border_string[i] = b.border_string[(norm+i+r)%norm];

    Border border_rotated = {new_border_string, 1};
    return border_rotated;
}