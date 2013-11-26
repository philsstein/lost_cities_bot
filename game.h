#ifndef LOST_CITIES_GAME_H
#define LOST_CITIES_GAME_H

#define MAX_NAME_SIZE 32
#define NUM_PLAYERS 2

#define HAND_SIZE 5
#define NUM_COLORS 5
#define NUM_COLOR_PLAYED 13
#define DECK_SIZE (NUM_COLORS * NUM_COLOR_PLAYED)

#define MAX_COLOR_SIZE 7 /* "yellow" */

typedef enum { RED=0, WHITE, BLUE, YELLOW, GREEN } color_t; 
static const char *color_map[] = { "R", "W", "B", "Y", "G" }; 

typedef struct {
    short value;     /* 1 == investement, 0 == empty slot, else value */
    color_t color; 
} card_t;

typedef struct {
    char name[MAX_NAME_SIZE]; 
    card_t hand[HAND_SIZE]; 
    card_t played[NUM_COLORS][NUM_COLOR_PLAYED]; 
} player_t;

typedef struct {
    player_t players[NUM_PLAYERS]; 
    card_t deck[DECK_SIZE];      
    card_t discard[DECK_SIZE]; 
    unsigned int top_deck; 
    unsigned int top_discard; 
} game_board_t;

int init_game_board(game_board_t *board); 
int add_player(game_board_t *, const char *name); 
void show_board(const game_board_t *board); 

const char *card_to_string(const card_t *); 

#endif /*  LOST_CITIES_GAME_H */


