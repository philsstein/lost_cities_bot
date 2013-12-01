#ifndef STACK_H_YUP_YESSIR
#define STACK_H_YUP_YESSIR

#include <assert.h>
#include <stdlib.h>

typedef enum { RED=0, WHITE, BLUE, YELLOW, GREEN } color_t; 
static const char *color_map[] = { "R", "W", "B", "Y", "G" }; 

typedef struct {
    short value;     /* 1 == investement, 0 == empty slot, else value */
    color_t color; 
} card_t;

typedef struct { 
    card_t *cards; 
    size_t max_size;
    unsigned int index;
} card_stack_t; 

const char *card_to_string(const card_t *);  /* returns pointer to static buffer */
const card_t *string_to_card(const char *);  /* returns pointer to static buffer */
const char *color_to_string(const color_t color); 
const char *stack_to_string(const card_stack_t *); 

void init_stack(card_stack_t *, const size_t max_size);
void deinit_stack(card_stack_t *); 

void push_stack(card_stack_t *, const card_t *); 
card_t *pop_stack(card_stack_t *); 
card_t *top_stack(const card_stack_t *); 

#endif 
