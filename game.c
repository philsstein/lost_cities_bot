#include <stdlib.h>   /* random() */
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "game.h"

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

int add_player(game_board_t *board, const char *name) {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (!board->players[i].name[0]) {
            strncpy(board->players[i].name, name, sizeof(board->players[i].name));
            for (int c = 0; c < HAND_SIZE; c++) {
                memcpy(&board->players[i].hand[c], &board->deck[board->top_deck], sizeof(card_t));
                memset(&board->deck[board->top_deck], 0, sizeof(card_t)); 
                board->top_deck++;
            }
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
            printf("%s ", card_to_string(&player->hand[c])); 
        printf("Played: [");
        for (int c = 0; c < NUM_COLORS; c++) {
            printf("%s: ", color_to_string(c)); 
            if (!player->played[c][0].value)
                printf("None ");
            else
                for (int v = 0; v != NUM_COLOR_PLAYED; v++) 
                    if (player->played[c][v].value) 
                        printf("%s ", card_to_string(&player->played[c][v])); 
        }
        printf("]\n");
    }
}

int init_game_board(game_board_t *board) {
    srandom(time(NULL));
    memset(board, 0, sizeof(*board)); 
    /* build deck from array of all cards */
    card_t allcards[DECK_SIZE] = {{ .value=1, .color=RED },  { .value=1, .color=RED },  { .value=1, .color=RED },  { .value=2, .color=RED },  { .value=3, .color=RED },  { .value=4, .color=RED },  { .value=5, .color=RED },  { .value=6, .color=RED },  { .value=7, .color=RED },  { .value=8, .color=RED },  { .value=9, .color=RED },  { .value=10, .color=RED },  { .value=1, .color=WHITE },  { .value=1, .color=WHITE },  { .value=1, .color=WHITE },  { .value=2, .color=WHITE },  { .value=3, .color=WHITE },  { .value=4, .color=WHITE },  { .value=5, .color=WHITE },  { .value=6, .color=WHITE },  { .value=7, .color=WHITE },  { .value=8, .color=WHITE },  { .value=9, .color=WHITE },  { .value=10, .color=WHITE },  { .value=1, .color=BLUE },  { .value=1, .color=BLUE },  { .value=1, .color=BLUE },  { .value=2, .color=BLUE },  { .value=3, .color=BLUE },  { .value=4, .color=BLUE },  { .value=5, .color=BLUE },  { .value=6, .color=BLUE },  { .value=7, .color=BLUE },  { .value=8, .color=BLUE },  { .value=9, .color=BLUE },  { .value=10, .color=BLUE },  { .value=1, .color=YELLOW },  { .value=1, .color=YELLOW },  { .value=1, .color=YELLOW },  { .value=2, .color=YELLOW },  { .value=3, .color=YELLOW },  { .value=4, .color=YELLOW },  { .value=5, .color=YELLOW },  { .value=6, .color=YELLOW },  { .value=7, .color=YELLOW },  { .value=8, .color=YELLOW },  { .value=9, .color=YELLOW },  { .value=10, .color=YELLOW },  { .value=1, .color=GREEN },  { .value=1, .color=GREEN },  { .value=1, .color=GREEN },  { .value=2, .color=GREEN },  { .value=3, .color=GREEN },  { .value=4, .color=GREEN },  { .value=5, .color=GREEN },  { .value=6, .color=GREEN },  { .value=7, .color=GREEN },  { .value=8, .color=GREEN },  { .value=9, .color=GREEN },  { .value=10, .color=GREEN }}; 

    /* now grab random cards from all cards to populate the deck. */
    for (int i = 0; i < DECK_SIZE; i++) {
        int r = random() % DECK_SIZE; 
        if (allcards[r].value != -1)  {
            memcpy(&board->deck[i], &allcards[r], sizeof(allcards[r]));
            allcards[r].value = -1; 
        }
    }

    return 1; 
}

