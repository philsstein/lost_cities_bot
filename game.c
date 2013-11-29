#include <string.h>
#include <stdio.h>
#include <time.h>
#include "game.h"

int add_player(game_board_t *board, const char *name) {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (!board->players[i].name[0]) {
            strncpy(board->players[i].name, name, sizeof(board->players[i].name));
            for (int c = 0; c < HAND_SIZE; c++) 
                push_stack(&board->players[i].hand, pop_stack(&board->deck));
            return 1;
        }
    }
    return 0; 
}

int play_card(game_board_t *board, const char *name, const char *cardstr) {
    player_t *player = NULL;
    for (int i=0; i < NUM_PLAYERS; i++)
        if (!strcmp(name, board->players[i].name)) {
            player = &board->players[i];
            break;
        }
    
    if (!player)
        return 0;

    for (int i = 0; i < HAND_SIZE; i++)
        if (strcmp(cardstr, card_to_string(&player->hand.cards[i]))) {
            card_t *card = &player->hand.cards[i];
            if (card->value > top_stack(&player->played[card->color])->value) {
                push_stack(player->played, card);
                return 1;
            }
        }
    return 0;
}

void show_board(const game_board_t *board) {
    printf("Game State:\n"); 
    for (int p = 0; p < NUM_PLAYERS; p++) {
        const player_t *player = &board->players[p];
        printf("Player: %s Hand: ", player->name); 
        for (int c = 0; c < HAND_SIZE; c++)
            printf("%s ", card_to_string(&player->hand.cards[c]));
        printf("Played: [");
        for (int c = 0; c < NUM_COLORS; c++) {
            printf("%s: ", color_to_string(c)); 
            if (!player->played[c].cards[0].value)
                printf("None ");
            else
                for (int v = 0; v != MAX_PLAYED_CARDS; v++) 
                    if (player->played[c].cards[v].value) 
                        printf("%s ", card_to_string(&player->played[c].cards[v])); 
        }
        printf("]\n");
    }
}

int init_game_board(game_board_t *board) {
    memset(board, 0, sizeof(*board)); 

    init_stack(&board->deck, DECK_SIZE); 
    init_stack(&board->discard, DECK_SIZE); 
    
    for (int p = 0; p < NUM_PLAYERS; p++) {
        init_stack(&board->players[p].hand, HAND_SIZE); 
        for (int c = 0; c < NUM_COLORS; c++) 
            init_stack(&board->players[p].played[c], MAX_PLAYED_CARDS); 
    }

    /* build deck from array of all cards */
    card_t allcards[DECK_SIZE] = {{ .value=1, .color=RED },  { .value=1, .color=RED },  { .value=1, .color=RED },  { .value=2, .color=RED },  { .value=3, .color=RED },  { .value=4, .color=RED },  { .value=5, .color=RED },  { .value=6, .color=RED },  { .value=7, .color=RED },  { .value=8, .color=RED },  { .value=9, .color=RED },  { .value=10, .color=RED },  { .value=1, .color=WHITE },  { .value=1, .color=WHITE },  { .value=1, .color=WHITE },  { .value=2, .color=WHITE },  { .value=3, .color=WHITE },  { .value=4, .color=WHITE },  { .value=5, .color=WHITE },  { .value=6, .color=WHITE },  { .value=7, .color=WHITE },  { .value=8, .color=WHITE },  { .value=9, .color=WHITE },  { .value=10, .color=WHITE },  { .value=1, .color=BLUE },  { .value=1, .color=BLUE },  { .value=1, .color=BLUE },  { .value=2, .color=BLUE },  { .value=3, .color=BLUE },  { .value=4, .color=BLUE },  { .value=5, .color=BLUE },  { .value=6, .color=BLUE },  { .value=7, .color=BLUE },  { .value=8, .color=BLUE },  { .value=9, .color=BLUE },  { .value=10, .color=BLUE },  { .value=1, .color=YELLOW },  { .value=1, .color=YELLOW },  { .value=1, .color=YELLOW },  { .value=2, .color=YELLOW },  { .value=3, .color=YELLOW },  { .value=4, .color=YELLOW },  { .value=5, .color=YELLOW },  { .value=6, .color=YELLOW },  { .value=7, .color=YELLOW },  { .value=8, .color=YELLOW },  { .value=9, .color=YELLOW },  { .value=10, .color=YELLOW },  { .value=1, .color=GREEN },  { .value=1, .color=GREEN },  { .value=1, .color=GREEN },  { .value=2, .color=GREEN },  { .value=3, .color=GREEN },  { .value=4, .color=GREEN },  { .value=5, .color=GREEN },  { .value=6, .color=GREEN },  { .value=7, .color=GREEN },  { .value=8, .color=GREEN },  { .value=9, .color=GREEN },  { .value=10, .color=GREEN }}; 

    /* now grab random cards from all cards to populate the deck. */
    for (int i = 0; i < DECK_SIZE; i++) {
        int r = random() % DECK_SIZE; 
        if (allcards[r].value != -1)  {
            push_stack(&board->deck, &allcards[r]); 
            allcards[r].value = -1; 
        }
    }

    return 1; 
}

