#include <stdio.h>
#include <string.h>
#include <libircclient.h>
#include "log.h"
#include "game_commands.h"
#include "irc_game.h"
#include "game.h"

/* utils and static functions */
static int show_table(game_board_t *board, irc_session_t * session, const char *whence)
{
    if (!is_game_active(board)) {
        irc_cmd_msg(session, whence, "The game has not yet started."); 
        return 1; 
    }

    for (int i = 0; i < 2; i++) {
        if (!get_played_cards(board, i)) {
            irc_cmd_msg(session, whence, "Error getting hand for player."); 
            return 0; 
        }
        else
            irc_cmd_msg(session, whence, board->response); 
    }
    if (!get_discards(board)) {
        irc_cmd_msg(session, whence, "Error getting discard pile."); 
        return 0; 
    }
    else
        irc_cmd_msg(session, whence, board->response); 
   
    char buf[64]; 
    snprintf(buf, sizeof(buf), "Cards remaining in draw deck: %d.", num_cards(&board->deck)); 
    irc_cmd_msg(session, whence, buf); 

    if (DRAW & board->valid_actions)
        snprintf(buf, sizeof(buf), "It is %s's turn to draw a card.",
                board->players[board->player_turn].name); 
    else
        snprintf(buf, sizeof(buf), "It is %s's turn to play or discard.",
                board->players[board->player_turn].name); 

    irc_cmd_msg(session, whence, buf); 

    return 1; 
}

/*
 * handle game commands from user. 
 */
static int handle_xyzzy(irc_session_t * session, const char * event, const char * origin,
        const char **params, unsigned int count)
{
    char nick[32];
    irc_target_get_nick(origin, nick, sizeof(nick)); 
    return irc_cmd_msg(session, nick, "Nothing happens."); 
}
static int handle_get_lost(irc_session_t * session, const char * event, const char * origin,
        const char **params, unsigned int count)
{
    return irc_cmd_quit(session, "Lost Cities bot getting lost...");
}
static int handle_join(irc_session_t * session, const char * event, const char * origin,
        const char **params, unsigned int count)
{
    char nick[32];
    irc_target_get_nick(origin, nick, sizeof(nick)); 
    game_board_t *game = &((game_board_list_t *)irc_get_ctx(session))->game; 
    const char *channel = ((game_board_list_t *)irc_get_ctx(session))->channel; 

    addlog("Attempting to add %s to game in channel %s.", nick, channel); 
    if (!add_player(game, nick)) 
        addlog("Unable to add % to game.", nick); 
    else {
        char buf[32]; 
        snprintf(buf, sizeof(buf), "+v %s", nick); 
        irc_cmd_channel_mode(session, channel, buf); 
        irc_cmd_msg(session, params[0], game->response); 
    }
    if (game->players[0].name[0] && game->players[1].name[0])
        show_table(game, session, params[0]); 

    return 1;
}
static int handle_help(irc_session_t * session, const char * event, const char * origin,
        const char **params, unsigned int count)
{
    char nick[32];
    irc_target_get_nick(origin, nick, sizeof(nick)); 
    if (!strncmp(params[1], "!help", sizeof("!help")))
        return irc_cmd_msg(session, nick, 
            "Valid commands: !join !table !play !discard !draw !hand. Each turn must be "
            "play [card] then !draw [color]. !help [cmd] for help on that command."); 

    const char *msg = "I don't have any help on that command."; 
    const char *cmd = strchr(params[1], ' '); 
    if (cmd) {
        for (int i = 0; i < num_game_commands; i++) {
            if (!strcmp(cmd+1, game_commands[i].cmd+1)) {  /* skip leading ! */
                msg = game_commands[i].help; 
                break;
            }
        }
    }
    return irc_cmd_msg(session, nick, msg); 
}

/* handle_play and handle_discard should be merged or something. */
static int handle_play(irc_session_t * session, const char * event, const char * origin,
        const char **params, unsigned int count)
{
    char nick[32];
    irc_target_get_nick(origin, nick, sizeof(nick)); 
    game_board_t *game = &((game_board_list_t *)irc_get_ctx(session))->game; 
    const char *card = strchr(params[1], ' '); 

    if (!card)
        return irc_cmd_msg(session, params[0], "You must choose a card to play."); 
    if (!play_card(game, nick, card+1)) 
        return irc_cmd_msg(session, params[0], "Error playing card."); 

    return irc_cmd_msg(session, params[0], game->response); 
}

static int handle_discard(irc_session_t * session, const char * event, const char * origin,
        const char **params, unsigned int count)
{
    char nick[32];
    irc_target_get_nick(origin, nick, sizeof(nick)); 
    game_board_t *game = &((game_board_list_t *)irc_get_ctx(session))->game; 
    const char *card = strchr(params[1], ' '); 
    if (!card)
        return irc_cmd_msg(session, params[0], "You must choose a card to discard."); 
    if (!discard(game, nick, card+1))
        return irc_cmd_msg(session, params[0], "Error discarding card.");

    return irc_cmd_msg(session, params[0], game->response); 
}

static int handle_hand(irc_session_t * session, const char * event, const char * origin,
        const char **params, unsigned int count)
{
    char nick[32];
    irc_target_get_nick(origin, nick, sizeof(nick)); 
    game_board_t *game = &((game_board_list_t *)irc_get_ctx(session))->game; 
    if (!get_player_hand(game, nick)) 
        return irc_cmd_msg(session, params[0], "Unable to get retrieve your hand.");

    return irc_cmd_msg(session, nick, game->response); 
}

static int handle_table(irc_session_t * session, const char * event, const char * origin,
        const char **params, unsigned int count)
{
    game_board_t *game = &((game_board_list_t *)irc_get_ctx(session))->game; 
    return show_table(game, session, params[0]); 
}

static int handle_draw(irc_session_t * session, const char * event, const char * origin,
        const char **params, unsigned int count)
{
    char nick[32];
    irc_target_get_nick(origin, nick, sizeof(nick)); 
    game_board_t *game = &((game_board_list_t *)irc_get_ctx(session))->game; 

    /* This string parsing may be better done inside game.c. */
    int draw_ret = 0; 
    if (params[1][5] == '\0') {
        draw_ret = draw_card(game, nick, DECK); 
        if (draw_ret && is_game_over(game)) {
            /* game over man! */
            irc_cmd_msg(session, nick, game->response); 
            if (!get_score(game))
                irc_cmd_msg(session, params[0], "Error getting final score."); 
            else {
                irc_cmd_msg(session, params[0], game->response); 
                deinit_game_board(game); 
                init_game_board(game); 
                return 0; 
            }
        }
    }
    else 
        switch (params[1][5]) {
            case 'R':
            case 'r': draw_ret = draw_card(game, nick, RED_PILE); break;
            case 'B':
            case 'b': draw_ret = draw_card(game, nick, BLUE_PILE); break;
            case 'W':
            case 'w': draw_ret = draw_card(game, nick, WHITE_PILE); break;
            case 'G':
            case 'g': draw_ret = draw_card(game, nick, GREEN_PILE); break;
            case 'Y':
            case 'y': draw_ret = draw_card(game, nick, YELLOW_PILE); break;
            default: 
                      draw_ret = 0;
        }

    if (!draw_ret)
        return irc_cmd_msg(session, params[0], "Unable to draw from that deck."); 

    return irc_cmd_msg(session, nick, game->response); 
}

/* externed in game_commands.h. this is the interface to game commands. */
game_command game_commands[] = {
    { .cmd="!xyzzy", .func=handle_xyzzy, .help=NULL},
    { .cmd="!get lost", .func=handle_get_lost, .help=NULL}, 
    { .cmd="!join", .func=handle_join, .help="!join - Join a game. Once two people have joined, "
                                            "the game will automatically begin."}, 
    { .cmd="!help", .func=handle_help, .help="!help - Display all game commands."}, 
    { .cmd="!play", .func=handle_play, .help="!play [card] - Play a card. Card is of the form "
                                            "shown in !hand."},
    { .cmd="!discard", .func=handle_discard, .help="!discard [card] - Discard a card. The card "
                                            "will be discarded into the appropriate color pile. "
                                                "The format of [card] is shown in !hand."}, 
    { .cmd="!hand", .func=handle_hand, .help="!hand - Show the current contents of your hand. "
                                            "Investment cards are shown as a star, e.g. R* is "
                                                "a red investment."}, 
    { .cmd="!table", .func=handle_table, .help="!table - show the current state of the table."},
    { .cmd="!draw", .func=handle_draw, .help="!draw [color] - Draw a card off the discard stack "
                                            "of the given color. If color is not given, draw "
                                                "off the draw stack."}
};
size_t num_game_commands = sizeof(game_commands)/sizeof(game_commands[0]); 

