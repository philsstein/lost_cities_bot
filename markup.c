#include <stdio.h>
#include <libircclient.h>
#include "markup.h"
#include "card_stack.h"   /* for color enum */

static markup_style_t markup_style = NO_MARKUP;

void set_markup_style(const markup_style_t style) 
{
    markup_style = style;
}

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

    if (markup_style == MIRC) {
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
    }
    else if (markup_style == XTERM) {
        /* red, white, blue, yellow, green */
        unsigned int xterm_color_map[] = { 31, 37, 34, 33, 32 }; 
        if (sizeof(buffer) <= snprintf(buffer, sizeof(buffer), "\033[%d;1m%s\033[0m", 
                    xterm_color_map[(int)color], str)) {
            return NULL;
        }
    }
    else if (markup_style == NO_MARKUP) {
        if (sizeof(buffer) <= snprintf(buffer, sizeof(buffer), "%s", str)) {
            return NULL;
        }
    }
    else /* cannot happen */
        return NULL; 

    return buffer; 
}


