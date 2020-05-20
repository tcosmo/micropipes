#include "border.h"

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