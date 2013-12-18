#ifndef STACK_H_YUP_YESSIR
#define STACK_H_YUP_YESSIR

#include <assert.h>
#include <stdlib.h>

/*
 * this defines a stack of cards. It has the normal stack type abilities, push, 
 * pop, top, etc. It also defines what a card looks like, color and value. 
 * The color is just an enum, to get a string value for it, use the _to_string
 * functions. In order to use a card stack, you must init it, then deinit it 
 * when you're done with it. 
 *
 */
typedef enum { RED=0, WHITE, BLUE, YELLOW, GREEN } color_t; 

typedef struct {
    short value;     /* 1 == investement, 0 == empty slot, else value */
    color_t color; 
} card_t;

/* do not mess with this. internal use only. */
typedef struct { 
    card_t *cards; 
    size_t max_size;
    unsigned int index;
} card_stack_t; 

/* convert cards to strings and vise versa. */
const char *card_to_markup_string(const card_t *);  /* returns pointer to static buffer */
const card_t *string_to_card(const char *);  /* returns pointer to static buffer */
const char *color_to_string(const color_t color); 
const char *stack_to_string(const card_stack_t *); 

/* 
 * create a new card stack of the given size. 
 */
void init_stack(card_stack_t *, const size_t max_size);
/* 
 * destroy the given card stack.
 */
void deinit_stack(card_stack_t *); 

/* push a new card on the stack. This function 
 * does *not* check if you push more cards than 
 * you have memoery for. So don't do that. 
 */
void push_stack(card_stack_t *, const card_t *); 
/*
 * Remove the top card from the stack and return a pointer to it.
 * The pointer is valid until you push another card on the stack.
 */
card_t *pop_stack(card_stack_t *); 
/* 
 * Return a pointer to the card on the top of the stack. The 
 * pointer will be valid and point to that card until you
 * pop it. 
 */
card_t *top_stack(const card_stack_t *); 
/*
 * Return how many cards are in the stack.
 */
unsigned int num_cards(const card_stack_t *); 

#endif 
