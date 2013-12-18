#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "game.h"
#include "card_stack.h"

int stack_test(void) {
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

    printf("top stack: %s\n", card_to_markup_string(top_stack(&card_stack))); 
    card_t *card = top_stack(&card_stack);
    assert(card->value == 6 && card->color == WHITE); 
    card = pop_stack(&card_stack); 
    printf("Popped: %s.\n", card_to_markup_string(card)); 
    card = pop_stack(&card_stack); 
    printf("Popped: %s. Should be 3 yellow.\n", card_to_markup_string(card)); 
    assert(card->value == 3 && card->color == YELLOW); 

    for (int i = 0; i < card_stack_size; i++) 
        push_stack(&card_stack, &c); 

    for (int i = 0; i < card_stack_size; i++) 
        pop_stack(&card_stack); 

    deinit_stack(&card_stack); 
    return 0;
}

int add_player_test(void) {
    game_board_t board;
    init_game_board(&board); 
    printf("sizeof(board): %lu\n", sizeof(board)); 
    printf("top of deck: %s\n", card_to_markup_string(top_stack(&board.deck))); 
    const char *names[] = { "Geoff", "Erica" }; 
    for (int i = 0; i < sizeof(names)/sizeof(names[0]); i++) {
        if (!add_player(&board, names[i])) {
            fprintf(stderr, "Error adding %s to the game.\n", names[i]);
            return 1; 
        }
        else
            printf("%s\n", board.response); 
    }
    /* adding same player a second time is not an error. */
    assert(add_player(&board, names[0])); 
    printf("%s\n", board.response); 

    assert(get_player(&board, names[0])); 
    assert(get_player(&board, names[1])); 
    assert(!get_player(&board, "johnsmith")); 

    card_t nocard; 
    memset(&nocard, 0, sizeof(nocard)); 
    assert(memcmp(&board.players[0].hand[0], &nocard, sizeof(card_t))); 
    assert(memcmp(&board.players[1].hand[4], &nocard, sizeof(card_t))); 

    show_board(&board);
    deinit_game_board(&board); 
    return 0;
}

int play_card_test(void) {
    game_board_t board;
    init_game_board(&board); 
    add_player(&board, "Geoff"); 
    add_player(&board, "Erica"); 

    player_t *p = &board.players[0]; 
    card_t cards[] = {
        {.value=1, .color=BLUE},
        {.value=3, .color=BLUE},
        {.value=2, .color=BLUE}}; 
    /* force p1 hand to be cvards given. */
    memcpy(p->hand, cards, sizeof(cards)); 
    const char *card_strs[] = { "B*", "B3", "B2" }; 

    /* First play is ok. */
    assert(play_card(&board, p->name, card_strs[0])); 
    printf("%s\n", board.response); 
    /* Should not be able to play again w/out drawing first. */
    assert(board.valid_actions == (DRAW)); 
    assert(!play_card(&board, p->name, card_strs[1])); 
    /* Other player should not be able to play until p1 draws. */
    assert(!discard(&board, board.players[1].name, card_to_markup_string(&board.players[1].hand[0]))); 
    assert(board.valid_actions == DRAW); 
    assert(draw_card(&board, p->name, DECK)); 
    /* first player completes turn. */
    assert(board.player_turn == 1); 
    assert(board.valid_actions == (PLAY | DISCARD)); 
    /* cheating here, making it the same player's turn. */
    board.player_turn = 0;
    printf("%s\n", board.response); 
    /* can play B3 on a B* */
    assert(play_card(&board, p->name, card_strs[1])); 
    printf("%s\n", board.response); 
    assert(draw_card(&board, p->name, DECK)); 
    board.player_turn = 0;
    printf("%s\n", board.response); 
    /* Cannot play a B2 on a B3 */
    assert(!play_card(&board, p->name, card_strs[2])); 
    /* no need to draw, we failed to play. */
    assert(discard(&board, p->name, card_strs[2])); 
    printf("%s\n", board.response); 
    assert(draw_card(&board, p->name, DECK)); 
    board.player_turn = 0;
    printf("%s\n", board.response); 

    /* Make sure all the draw_cards worked */
    for (int c = 0; c < HAND_SIZE; c++) 
        assert(p->hand[c].value != 0); 

    show_board(&board); 

    deinit_game_board(&board); 
    return 0;
}

int draw_card_test(void) {
    game_board_t board;
    init_game_board(&board); 
    add_player(&board, "Rhen"); 
    add_player(&board, "Shtimpy"); 
    board.player_turn = 0;

    player_t *p0 = &board.players[0]; 
    player_t *p1 = &board.players[1]; 

    card_t card = { .value=5, .color=BLUE }; 
    memcpy(&board.players[0].hand[0], &card, sizeof(card)); 
    card.color = RED; 
    memcpy(&board.players[1].hand[0], &card, sizeof(card)); 

    assert(discard(&board, p0->name, "B5")); 
    /* draw from deck */
    assert(draw_card(&board, p0->name, DECK)); 

    assert(discard(&board, p1->name, "R5")); 
    /* draw from board, the B5 just discarded */
    assert(draw_card(&board, p1->name, BLUE_PILE)); 

    deinit_game_board(&board); 
    return 0;
}
    
int main(void) {
    srandom(1); 
   
    struct {
        int (*func)(void); 
        const char *name;
    } tests [] = { 
        { .func = stack_test, .name = "stack test" }, 
        { .func = add_player_test, .name = "add player test" }, 
        { .func = play_card_test, .name = "play card test" },
        { .func = draw_card_test, .name = "draw card test" },
    }; 
    for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("=======Running test: %s\n", tests[i].name); 
        if ((*tests[i].func)()) 
            fprintf(stderr, "Error in test: %s\n", tests[i].name); 
        else
            printf("OK\n"); 
    }

    return EXIT_SUCCESS; 
}
