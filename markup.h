#ifndef IN_LIVING_COLOR_H
#define IN_LIVING_COLOR_H

#include "card_stack.h"

/* 
 * returns pointer to static buffer with markup 
 * applied to the string given.
 */
const char *markup(const color_t color, const char *str); 

#endif /*fndef IN_LIVING_COLOR_H */
