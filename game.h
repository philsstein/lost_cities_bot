#ifndef LOST_CITIES_GAME_H
#define LOST_CITIES_GAME_H

#include "card_stack.h"

#define MAX_NAME_SIZE 32  /* RFC 2812 says max nick is 9 so this may be overkill. */
#define NUM_PLAYERS 2
#define HAND_SIZE 5
#define NUM_COLORS 5
#define MAX_PLAYED_CARDS 12
#define DECK_SIZE (NUM_COLORS * MAX_PLAYED_CARDS)
#define MAX_COLOR_SIZE 7 /* "yellow" */
#define MAX_RESP_SIZE 256

/* Keep track of valid plays. */
enum { PLAY = 1, DISCARD = 2, DRAW = 4 };

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
    char valid_actions; 
    char response[MAX_RESP_SIZE]; 
} game_board_t;

/* game commands */
int add_player(game_board_t *, const char *name); 
int play_card(game_board_t *board, const char *name, const char *cardstr); 
int discard(game_board_t *board, const char *name, const char *cardstr); 
player_t *get_player(game_board_t *board, const char *name); 
int get_player_hand(game_board_t *board, const char *name); 
int get_played_cards(game_board_t *board, const unsigned short player_num); 
int get_discards(game_board_t *board); 

typedef enum { DECK, RED_PILE, WHITE_PILE, BLUE_PILE, YELLOW_PILE, GREEN_PILE } draw_pile_t; 
int draw_card(game_board_t *board, const char *name, const draw_pile_t); 

int is_game_active(const game_board_t *board); 
int is_game_over(const game_board_t *board); 
int get_score(game_board_t *board); 

/* Can be called anytime. Will only return non-zero if player is in game and is replaced. */
int replace_player(game_board_t *board, const char *cur_name, const char *new_name); 

/* init / deinit structures */
int init_game_board(game_board_t *board); 
int deinit_game_board(game_board_t *board); 
int load_config(const char *conf_file); 

/* debug / logging */
void show_board(const game_board_t *board); 

#endif /*  LOST_CITIES_GAME_H */


