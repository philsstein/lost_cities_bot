#ifndef LOST_CITIES_GAME_H
#define LOST_CITIES_GAME_H

#include "card_stack.h"

#define MAX_NAME_SIZE 32
#define NUM_PLAYERS 2

#define HAND_SIZE 5
#define NUM_COLORS 5
#define MAX_PLAYED_CARDS 13
#define DECK_SIZE (NUM_COLORS * MAX_PLAYED_CARDS)

#define MAX_COLOR_SIZE 7 /* "yellow" */

typedef struct {
    char name[MAX_NAME_SIZE]; 
    card_stack_t hand; 
    card_stack_t played[NUM_COLORS];
} player_t;

typedef struct {
    player_t players[NUM_PLAYERS]; 
    card_stack_t deck;
    card_stack_t discard; 
} game_board_t;

int init_game_board(game_board_t *board); 
int deinit_game_board(game_board_t *board); 

int add_player(game_board_t *, const char *name); 
void show_board(const game_board_t *board); 

#endif /*  LOST_CITIES_GAME_H */


