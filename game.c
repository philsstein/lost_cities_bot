#include <stdlib.h>
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
    if (!card)
        return 0; 
    for (int i = 0; i < HAND_SIZE; i++) {
        if (card->value == player->hand[i].value && card->color == player->hand[i].color) {
            if (!discard) {   /* play the card */
                const card_t *top_card = top_stack(&player->played[card->color]); 
                if (!top_card || card->value > top_card->value) {
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

int get_player_hand(game_board_t *board, const char *name) 
{
    player_t *p; 
    if ((p = get_player(board, name)) == NULL) {
        snprintf(board->response, sizeof(board->response), "%s", 
                "You are not in the game."); 
        return 0; 
    }

    int i = 0; 
    for (int c = 0; c < HAND_SIZE; c++)
        i += snprintf(board->response + i, sizeof(board->response) - i, 
                "%s ", card_to_markup_string(&p->hand[c])); 
    return 1;
}

int get_played_cards(game_board_t *board, const unsigned short player_num)
{
    player_t *p = &board->players[player_num]; 
    unsigned int buf_i = 0; 
    buf_i += snprintf(board->response + buf_i, sizeof(board->response) - buf_i, 
            "%s has played [", p->name); 
    for (int c = 0; c < NUM_COLORS; c++) 
        buf_i += snprintf(board->response + buf_i, sizeof(board->response) - buf_i, 
                "%s: %s", color_to_string(c), stack_to_string(&p->played[c])); 
    buf_i += snprintf(board->response + buf_i, sizeof(board->response) - buf_i, "]");
    return 1; 
}

int get_discards(game_board_t *board) 
{
    int buf_i = 0; 
    buf_i += snprintf(board->response + buf_i, sizeof(board->response) - buf_i, "Discards: ["); 
    for (int c = 0; c < NUM_COLORS; c++) 
        buf_i += snprintf(board->response + buf_i, sizeof(board->response) - buf_i, 
                "%s: %s", color_to_string(c), stack_to_string(&board->discard[c])); 
    buf_i += snprintf(board->response + buf_i, sizeof(board->response) - buf_i, "]"); 
    return 1; 
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
                "There are enough players! The game is started and %s goes first.", 
                board->players[board->player_turn].name); 

    return buf_used; 
}

static int turn_check(game_board_t *board, const char *name) {
    player_t *p = get_player(board, name); 
    if (!p) 
        return 0; 
    if (strncmp(board->players[board->player_turn].name, p->name, MAX_NAME_SIZE)) {
        snprintf(board->response, sizeof(board->response), "%s", "It is not your turn."); 
        return 0;
    }
    return 1;
}

int discard(game_board_t *board, const char *name, const char *cardstr) {
    if (!(DISCARD & board->valid_actions) || !turn_check(board, name)) 
        return 0; 
    int ret_val = from_hand_to_stack(board, name, cardstr, 1); 
    if (!ret_val) 
        (void)snprintf(board->response, sizeof(board->response), 
                "Error discaring %s.", cardstr);
    else if (ret_val) {
        (void)snprintf(board->response, sizeof(board->response), 
                "%s discarded %s.", name, cardstr);
        board->valid_actions = DRAW; 
    }
    return ret_val;
}

int play_card(game_board_t *board, const char *name, const char *cardstr) {
    if (!(PLAY & board->valid_actions) || !turn_check(board, name)) 
        return 0; 
    int ret_val = from_hand_to_stack(board, name, cardstr, 0); 
    if (!ret_val) 
        (void)snprintf(board->response, sizeof(board->response), 
                "Error playing %s.", cardstr);
    else if (ret_val) {
        (void)snprintf(board->response, sizeof(board->response), 
                "%s played %s.", name, card_to_markup_string(string_to_card(cardstr))); 
        board->valid_actions = DRAW; 
    }
    return ret_val; 
}

int draw_card(game_board_t *board, const char *name, const draw_pile_t pile) {
    if (!(DRAW & board->valid_actions) || !turn_check(board, name)) 
        return 0; 
    player_t *player = get_player(board, name); 
    for (int i = 0; i < HAND_SIZE; i++) 
        if (player->hand[i].value == 0) {
            card_stack_t *stack; 
            switch (pile) {
                case DECK: stack = &board->deck; break;
                case RED_PILE: stack = &board->discard[RED]; break;
                case WHITE_PILE: stack = &board->discard[WHITE]; break;
                case YELLOW_PILE: stack = &board->discard[YELLOW]; break;
                case BLUE_PILE: stack = &board->discard[BLUE]; break;
                case GREEN_PILE: stack = &board->discard[GREEN]; break;
            }
            const card_t *card = pop_stack(stack); 
            if (card) {
                memcpy(&player->hand[i], card, sizeof(card_t)); 
                snprintf(board->response, sizeof(board->response), 
                        "You drew card %s.", card_to_markup_string(card)); 
                board->valid_actions = PLAY | DISCARD; 
                board->player_turn = board->player_turn ? 0 : 1;
                return 1;
            }
            else
                return 0; 
        }
    
    return 0; 
}

int is_game_active(const game_board_t *board)
{
    return board->players[1].name[0]; 
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
            printf("%s ", card_to_markup_string(&player->hand[c]));
        printf("Played: [");
        for (int c = 0; c < NUM_COLORS; c++) 
            printf("%s: %s", color_to_string(c), stack_to_string(&player->played[c])); 
        printf("]\n");
    }
}

int replace_player(game_board_t *board, const char *old_name, const char *new_name)
{
    player_t *player; 
    if ((player = get_player(board, old_name)) == NULL)
        return 0; 

    strncpy(player->name, new_name, sizeof(player->name)); 
    snprintf(board->response, sizeof(board->response), 
            "Replaced in-game Lost Cities player %s with %s.", old_name, new_name); 
    return 1;
}

int init_game_board(game_board_t *board) {
    memset(board, 0, sizeof(*board)); 
    board->player_turn = random() % 2; 
    board->valid_actions = PLAY | DISCARD; 
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
