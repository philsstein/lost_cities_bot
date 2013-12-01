#ifndef LOST_CITIES_GAME_H
#define LOST_CITIES_GAME_H

#include "card_stack.h"

#define MAX_NAME_SIZE 32
#define NUM_PLAYERS 2

#define HAND_SIZE 5
#define NUM_COLORS 5
#define MAX_PLAYED_CARDS 12
#define DECK_SIZE (NUM_COLORS * MAX_PLAYED_CARDS)

#define MAX_COLOR_SIZE 7 /* "yellow" */

typedef struct {
    char name[MAX_NAME_SIZE]; 
    card_t hand[HAND_SIZE]; 
    card_stack_t played[NUM_COLORS];
} player_t;

typedef struct {
    player_t players[NUM_PLAYERS]; 
    unsigned short player_turn;
    card_stack_t deck;
    card_stack_t discard[NUM_COLORS]; 
} game_board_t;

/* init / deinit structures */
int init_game_board(game_board_t *board); 
int deinit_game_board(game_board_t *board); 
int load_config(const char *conf_file); 

/* game commands */
int add_player(game_board_t *, const char *name); 
int play_card(game_board_t *board, const char *name, const char *cardstr); 
int discard(game_board_t *board, const char *name, const char *cardstr); 
player_t *get_player(game_board_t *board, const char *name); 
int draw_card(game_board_t *board, const char *name); 

/* debug / logging */
void show_board(const game_board_t *board); 

#endif /*  LOST_CITIES_GAME_H */


