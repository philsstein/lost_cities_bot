#include <string.h>
#include "log.h"
#include "irc_game.h"
#include "game_commands.h"

void dump_event (irc_session_t * session, const char * event, const char * origin, 
        const char ** params, unsigned int count) 
{
    char buf[512];
    int cnt;

    buf[0] = '\0';

    for ( cnt = 0; cnt < count; cnt++ ) {
        if ( cnt )
            strcat (buf, "|");

        strcat (buf, params[cnt]);
    }

    addlog("Event \"%s\", origin: \"%s\", params: %d [%s]", event, 
            origin ? origin : "NULL", cnt, buf);
}

/****************************************** 
 * handle IRC events
 ******************************************/
void event_connect(irc_session_t * session, const char * event, const char * origin,
        const char ** params, unsigned int count) 
{
    dump_event(session, event, origin, params, count);
    game_board_list_t *ctx = (game_board_list_t *)irc_get_ctx(session);
    addlog("Joining channel %s\n", ctx->channel); 
    irc_cmd_join(session, ctx->channel, 0);
}

void event_join(irc_session_t * session, const char * event, const char * origin,
        const char ** params, unsigned int count) 
{
    dump_event(session, event, origin, params, count); 
    irc_cmd_user_mode(session, "+i");
    if (origin && session) {
        char nickbuf[128]; 
        irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));
        game_board_list_t *ctx = (game_board_list_t *)irc_get_ctx(session);
        const char *bot_nick = ctx->bot_nick;
        if (!strncmp(nickbuf, bot_nick, sizeof(nickbuf))) {
            irc_cmd_msg(session, params[0], "Let's get this exploring party started!"); 
            irc_cmd_user_mode(session, "+o");
        }
    }
}
void event_nick(irc_session_t * session, const char * event, const char * origin,
        const char ** params, unsigned int count)
{
    dump_event(session, event, origin, params, count); 

    if (origin && session) {
        char nickbuf[128]; 
        irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));
        game_board_t *game = &((game_board_list_t *)irc_get_ctx(session))->game; 
        if (replace_player(game, nickbuf, params[0]))
            irc_cmd_msg(session, params[0], game->response); 
    }
}

void event_channel(irc_session_t * session, const char * event, const char * origin,
        const char ** params, unsigned int count) 
{
    dump_event(session, event, origin, params, count); 

    if (count != 2 || !origin || !params[1] == '!')
        return;

    int i; 
    for (i = 0; i < num_game_commands; i++) {
        /* if (!strncmp(params[1], game_commands[i].cmd, sizeof(game_commands[i].cmd)-1)) { */
        if (!strncmp(params[1], game_commands[i].cmd, strlen(game_commands[i].cmd))) {
            if (!game_commands[i].func(session, event, origin, params, count)) {
                addlog("Error running command %s", params[1]); 
            }
            break;
        }
    }
    if (i == num_game_commands) 
        irc_cmd_msg(session, params[0], "I don't know what you're talking about.");
}

void event_numeric (irc_session_t * session, unsigned int event, const char * origin,
        const char ** params, unsigned int count)
{
	if ( event > 400 )
	{
		addlog("ERROR %d: %s: %s %s %s %s\n", 
			event,
			origin ? origin : "unknown",
			params[0],
			count > 1 ? params[1] : "",
			count > 2 ? params[2] : "",
			count > 3 ? params[3] : "");
	}
}
