#include <string.h>  /* memcpy */ 
#include <stdio.h>
#include "card_stack.h"

static const char *color_map[] = { "R", "W", "B", "Y", "G" }; 

const char *color_to_string(const color_t color) {
    return color_map[color]; 
}

const char*card_to_string(const card_t *card) {
    static char buffer[16];
    if (card->value != 1)
        snprintf(buffer, sizeof(buffer), "%s%d", color_to_string(card->color), card->value); 
    else
        snprintf(buffer, sizeof(buffer), "%s*", color_to_string(card->color)); 

    return buffer;
}

const card_t *string_to_card(const char *str) {
    static card_t card; 
    switch (str[0]) {
        case 'R': card.color = RED; break; 
        case 'W': card.color = WHITE; break; 
        case 'B': card.color = BLUE; break; 
        case 'Y': card.color = YELLOW; break; 
        case 'G': card.color = GREEN; break; 
        default: return NULL;
    }
    switch (str[1]) {
        case '*': card.value = 1; break;
        case '1': card.value = 10; break;
        case '2': card.value = 2; break;
        case '3': card.value = 3; break;
        case '4': card.value = 4; break;
        case '5': card.value = 5; break;
        case '6': card.value = 6; break;
        case '7': card.value = 7; break;
        case '8': card.value = 8; break;
        case '9': card.value = 9; break;
        default: return NULL;
    }
    return &card;
}

const char *stack_to_string(const card_stack_t *stack) {
    static char buffer[256];
    buffer[0] = 0;
    int count = 0;
    for (int i = 0; i < stack->index; i++) {
        const char *cardstr = card_to_string(&stack->cards[i]);
        if (count + strnlen(cardstr, sizeof(buffer)) >= sizeof(buffer))
            return NULL; 
        count += snprintf(&buffer[count], sizeof(buffer)-count, "%s ", cardstr); 
    }
    return (const char*)&buffer; 
}

void init_stack(card_stack_t *stack, const size_t max_size) {
    stack->cards = (card_t*)malloc(max_size * sizeof(card_t)); 
    stack->max_size = max_size; 
    stack->index = 0;
}

void push_stack(card_stack_t *stack, const card_t *data) {
    assert(stack->index < stack->max_size); 
    memcpy(stack->cards + stack->index, data, sizeof(stack->cards[0])); 
    stack->index++; 
}

card_t *pop_stack(card_stack_t *stack) {
    if (stack->index == 0) 
        return NULL; 
    stack->index--; 
    return stack->cards + stack->index; 
}

card_t *top_stack(const card_stack_t *stack) {
    return stack->cards + stack->index-1; 
}

void deinit_stack(card_stack_t *stack) {
    free(stack->cards);
    stack->max_size = 0;
    stack->index = 0; 
}

