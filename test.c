#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "game.h"
#include "card_stack.h"

int main(void) {
    srandom(time(NULL)); 
    /*** stack tests ***/
    const unsigned int card_stack_size = 3;
    card_stack_t card_stack; 
    init_stack(&card_stack, card_stack_size); 

    card_t c = {.value=3, .color=YELLOW}; 
    push_stack(&card_stack, &c); 
    assert(card_stack.cards[0].value == 3 && card_stack.cards[0].color == YELLOW); 
    c.value = 6; 
    c.color = WHITE;
    push_stack(&card_stack, &c); 
    assert(card_stack.cards[1].value == 6 && card_stack.cards[1].color == WHITE); 

    printf("top stack: %s\n", card_to_string(top_stack(&card_stack))); 
    card_t *card = top_stack(&card_stack);
    assert(card->value == 6 && card->color == WHITE); 
    card = pop_stack(&card_stack); 
    printf("Popped: %s.\n", card_to_string(card)); 
    card = pop_stack(&card_stack); 
    printf("Popped: %s. Should be 3 yellow.\n", card_to_string(card)); 
    assert(card->value == 3 && card->color == YELLOW); 

    for (int i = 0; i < card_stack_size; i++) 
        push_stack(&card_stack, &c); 

    for (int i = 0; i < card_stack_size; i++) 
        pop_stack(&card_stack); 

    deinit_stack(&card_stack); 

    /***** game test ****/
    game_board_t board;
    init_game_board(&board); 
    printf ("deck: "); 
    for (int i = 0; i < 5; i++)
        printf(" %s", card_to_string(&board.deck.cards[i]));  
    printf(" ...\n"); 
    printf("sizeof(board): %lu\n", sizeof(board)); 
    printf("top of deck: %s\n", card_to_string(top_stack(&board.deck))); 
    const char *names[] = { "Geoff", "Erica" }; 
    for (int i = 0; i < sizeof(names)/sizeof(names[0]); i++) {
        if (!add_player(&board, names[i])) {
            fprintf(stderr, "Error adding %s to the game.\n", names[i]);
            return 1; 
        }
        else
            printf("Added %s to the game.\n", names[i]);
    }
    /* add a few card to player played stack */
    for (int i = 0; i < 5; i++) {
        card_t *c = pop_stack(&board.deck); 
        push_stack(&board.players[0].played[c->color], c); 
    }
    show_board(&board);

    return EXIT_SUCCESS; 
}
