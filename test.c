#include <stdio.h>
#include <string.h>
#include "game.h"

int main(void) {
    game_board_t board;
    init_game_board(&board); 
    printf ("deck: "); 
    for (int i = 0; i < 5; i++)
        printf(" %s", card_to_string(&board.deck[i])); 
    printf(" ...\n"); 
    printf("sizeof(board): %lu\n", sizeof(board)); 
    printf("top index: %d\n", board.top_deck); 
    printf ("top deck: %s\n", card_to_string(&board.deck[board.top_deck])); 
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
        card_t *c = &board.deck[board.top_deck]; 
        memcpy(&board.players[0].played[c->color][0], c, sizeof(board.deck[0])); 
        board.top_deck++; 
    }
    show_board(&board);
    return 0; 
}
