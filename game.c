#include <string.h>
#include <stdio.h>
#include <time.h>
#include "game.h"

/* Move card given from hand to appropriate card stack. Leave empty card in hand. */
static int from_hand_to_stack(game_board_t *board, const char *name, const char *cardstr,
                       unsigned short discard) {
    player_t *player; 
    if ((player = get_player(board, name)) == NULL)
        return 0; 
    
    const card_t *card = string_to_card(cardstr);  
    for (int i = 0; i < HAND_SIZE; i++) {
        if (card->value == player->hand[i].value && card->color == player->hand[i].color) {
            if (!discard) {   /* play the card */
                if (card->value > top_stack(&player->played[card->color])->value) {
                    push_stack(&player->played[card->color], card);
                    memset(&player->hand[i], 0, sizeof(card_t)); 
                    return 1;
                }
            }
            else {
                push_stack(&board->discard[card->color], card); 
                memset(&player->hand[i], 0, sizeof(card_t)); 
                return 1;
            }
        }
    }
    return 0;
}

player_t *get_player(game_board_t *board, const char *name) {
    for (int i=0; i < NUM_PLAYERS; i++) {
        if (!strcmp(name, board->players[i].name)) {
            return &board->players[i];
        }
    }
    return NULL; 
}

int add_player(game_board_t *board, const char *name) {
    int buf_used = 0;
    if (board->players[0].name[0] && board->players[1].name[0]) {
        buf_used += snprintf(board->response + buf_used, sizeof(board->response) - buf_used, 
                "There are already enough players in the game.");
        return buf_used;
    }
    else {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (!board->players[i].name[0]) {
                strncpy(board->players[i].name, name, sizeof(board->players[i].name));
                for (int c = 0; c < HAND_SIZE; c++) 
                    memcpy(&board->players[i].hand[c], pop_stack(&board->deck), sizeof(card_t)); 

                buf_used = snprintf(board->response, sizeof(board->response), 
                        "Added %s to the game. ", name); 
                break;
            }
            else if (!strncmp(board->players[i].name, name, sizeof(board->players[i].name))) {
                buf_used = snprintf(board->response, sizeof(board->response), 
                        "%s is already in the game. ", name); 
                return buf_used;
            }
        }
    }
    if (board->players[0].name[0] && board->players[1].name[0]) 
        buf_used += snprintf(board->response + buf_used, sizeof(board->response) - buf_used, 
                "There are enough players! The game has begun!");

    return buf_used; 
}

int discard(game_board_t *board, const char *name, const char *cardstr) {
    int ret_val = from_hand_to_stack(board, name, cardstr, 1); 
    if (ret_val) 
        (void)snprintf(board->response, sizeof(board->response), 
                "%s discarded %s.", name, cardstr);
    return ret_val;
}

int play_card(game_board_t *board, const char *name, const char *cardstr) {
    int ret_val = from_hand_to_stack(board, name, cardstr, 0); 
    if (ret_val) 
        (void)snprintf(board->response, sizeof(board->response), 
                "%s played %s.", name, cardstr);
    return ret_val; 
}

int draw_card(game_board_t *board, const char *name) {
    player_t *player; 
    if ((player = get_player(board, name)) == NULL)
        return 0; 

    for (int i = 0; i < HAND_SIZE; i++) 
        if (player->hand[i].value == 0) {
            const card_t *card = pop_stack(&board->deck); 
            if (card) {
                memcpy(&player->hand[i], card, sizeof(card_t)); 
                snprintf(board->response, sizeof(board->response), 
                        "%s drew card %s.", name, card_to_string(card)); 
                return 1;
            }
            else
                return 0; 
        }
    
    return 0; 
}

int load_config(const char *conf_file) {
    return 1; 
}

void show_board(const game_board_t *board) {
    printf("Game State:\n"); 
    printf("Deck: %s\n", stack_to_string(&board->deck)); 
    printf("Discards: ["); 
    for (int c = 0; c < NUM_COLORS; c++) 
        printf("%s: %s", color_to_string(c), stack_to_string(&board->discard[c]));
    printf("]\n");

    for (int p = 0; p < NUM_PLAYERS; p++) {
        const player_t *player = &board->players[p];
        printf("Player: %s Hand: ", player->name); 
        for (int c = 0; c < HAND_SIZE; c++)
            printf("%s ", card_to_string(&player->hand[c]));
        printf("Played: [");
        for (int c = 0; c < NUM_COLORS; c++) 
            printf("%s: %s", color_to_string(c), stack_to_string(&player->played[c])); 
        printf("]\n");
    }
}

int replace_player(game_board_t *board, const char *old_name, const char *new_name)
{
    for (int p = 0; p < NUM_PLAYERS; p++) {
        if (board->players[p].name[0]) {
            if (!strncmp(board->players[p].name, old_name, sizeof(board->players->name))) {
                strncpy(board->players[p].name, new_name, sizeof(board->players->name)); 
                snprintf(board->response, sizeof(board->response), 
                        "Replaced in-game Lost Cities player %s with %s.", old_name, new_name); 
                return 1;
            }
        }
    }
    return 0; 
}

int init_game_board(game_board_t *board) {
    memset(board, 0, sizeof(*board)); 
    board->player_turn = random() % 2; 
    init_stack(&board->deck, DECK_SIZE); 
    for (int p = 0; p < NUM_PLAYERS; p++) {
        for (int c = 0; c < NUM_COLORS; c++) {
            init_stack(&board->players[p].played[c], MAX_PLAYED_CARDS); 
        }
    }
    for (int c = 0; c < NUM_COLORS; c++) 
            init_stack(&board->discard[c], MAX_PLAYED_CARDS); 

    /* build deck from array of all cards */
    card_t allcards[] = {{ .value=1, .color=RED },  { .value=1, .color=RED },  { .value=1, .color=RED },  { .value=2, .color=RED },  { .value=3, .color=RED },  { .value=4, .color=RED },  { .value=5, .color=RED },  { .value=6, .color=RED },  { .value=7, .color=RED },  { .value=8, .color=RED },  { .value=9, .color=RED },  { .value=10, .color=RED },  { .value=1, .color=WHITE },  { .value=1, .color=WHITE },  { .value=1, .color=WHITE },  { .value=2, .color=WHITE },  { .value=3, .color=WHITE },  { .value=4, .color=WHITE },  { .value=5, .color=WHITE },  { .value=6, .color=WHITE },  { .value=7, .color=WHITE },  { .value=8, .color=WHITE },  { .value=9, .color=WHITE },  { .value=10, .color=WHITE },  { .value=1, .color=BLUE },  { .value=1, .color=BLUE },  { .value=1, .color=BLUE },  { .value=2, .color=BLUE },  { .value=3, .color=BLUE },  { .value=4, .color=BLUE },  { .value=5, .color=BLUE },  { .value=6, .color=BLUE },  { .value=7, .color=BLUE },  { .value=8, .color=BLUE },  { .value=9, .color=BLUE },  { .value=10, .color=BLUE },  { .value=1, .color=YELLOW },  { .value=1, .color=YELLOW },  { .value=1, .color=YELLOW },  { .value=2, .color=YELLOW },  { .value=3, .color=YELLOW },  { .value=4, .color=YELLOW },  { .value=5, .color=YELLOW },  { .value=6, .color=YELLOW },  { .value=7, .color=YELLOW },  { .value=8, .color=YELLOW },  { .value=9, .color=YELLOW },  { .value=10, .color=YELLOW },  { .value=1, .color=GREEN },  { .value=1, .color=GREEN },  { .value=1, .color=GREEN },  { .value=2, .color=GREEN },  { .value=3, .color=GREEN },  { .value=4, .color=GREEN },  { .value=5, .color=GREEN },  { .value=6, .color=GREEN },  { .value=7, .color=GREEN },  { .value=8, .color=GREEN },  { .value=9, .color=GREEN },  { .value=10, .color=GREEN }}; 

    assert(sizeof(allcards) == sizeof(card_t) * DECK_SIZE); 

    /* now grab random cards from all cards to populate the deck. */
    for (int i = 0; i < DECK_SIZE; i++) {
        while (1) {  /* scary */
            int r = random() % DECK_SIZE; 
            if (allcards[r].value != -1)  {
                push_stack(&board->deck, &allcards[r]); 
                allcards[r].value = -1; 
                break;
            }
        }
    }

    return 1; 
}

int deinit_game_board(game_board_t *board) {
    deinit_stack(&board->deck); 
    
    for (int p = 0; p < NUM_PLAYERS; p++) {
        for (int c = 0; c < NUM_COLORS; c++) 
            deinit_stack(&board->players[p].played[c]); 
    }
    for (int c = 0; c < NUM_COLORS; c++) 
        deinit_stack(&board->discard[c]); 

    return 1; 
}
