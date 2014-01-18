#ifndef IN_LIVING_COLOR_H
#define IN_LIVING_COLOR_H

#include "card_stack.h"

/* 
 * returns pointer to static buffer with markup 
 * applied to the string given.
 */
const char *markup(const color_t color, const char *str); 

/*
 * Set markup style. All calls after this will
 * use the style given.
 */
typedef enum { NO_MARKUP, MIRC, XTERM } markup_style_t; 
void set_markup_style(const markup_style_t style); 

#endif /*fndef IN_LIVING_COLOR_H */
