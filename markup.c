#include <stdio.h>
#include <libircclient.h>
#include "card_stack.h"   /* for color enum */

static const char* color_to_markup(const color_t c)
{
    switch (c) {
        case RED: return "RED"; break;
        case WHITE: return "WHITE"; break;
        case BLUE: return "BLUE"; break;
        case YELLOW: return "YELLOW"; break;
        case GREEN: return "GREEN"; break;
        default: return ""; break;
    }
}

const char *markup(const color_t color, const char *str) 
{
    static char buffer[128]; 

    if (sizeof(buffer) <= snprintf(buffer, sizeof(buffer), "[COLOR=%s]%s[/COLOR]", 
                color_to_markup(color), str)) {
        return NULL;
    }

    /* there is a better way to do this... */
    char *colored = irc_color_convert_to_mirc(buffer); 
    if (sizeof(buffer) <= snprintf(buffer, sizeof(buffer), "%s", colored)) {
        free(colored);
        return NULL;
    }
    free(colored);  

    return buffer; 
}


