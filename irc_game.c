#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "irc_game.h"

static void addlog (const char * fmt, ...) 
{
    FILE * fp;
    char buf[1024];
    va_list va_alist;

    va_start (va_alist, fmt);
#if defined (_WIN32)
    _vsnprintf (buf, sizeof(buf), fmt, va_alist);
#else
    vsnprintf (buf, sizeof(buf), fmt, va_alist);
#endif
    va_end (va_alist);

    printf ("%s\n", buf);

    if ( (fp = fopen ("irc_game.log", "ab")) != 0 ) {
        fprintf (fp, "%s\n", buf);
        fclose (fp);
    }
}

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
        /* irc_cmd_user_mode(session, "+o"); */
        char nickbuf[128]; 
        irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));
        const char *bot_nick = ((game_board_list_t *)irc_get_ctx(session))->bot_nick;
        if (!strncmp(nickbuf, bot_nick, sizeof(nickbuf))) 
            irc_cmd_msg(session, params[0], "Let's get this exploring party started!"); 
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
    char nickbuf[128];
    dump_event(session, event, origin, params, count); 

    if (count != 2)
        return;

    if (!origin)
        return;

    if (!params[1] == '!') 
        return;

    irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));
    game_board_list_t *ctx = (game_board_list_t *)irc_get_ctx(session);
    game_board_t *game = &ctx->game;  /* GTL one game for now. */
    const char *nick = (const char*)&nickbuf; 
    const char *channel = (const char*)&ctx->channel; 

    if (!strcmp(params[1], "!join")) {
        addlog("Attempting to add %s to game in channel %s.", nick, channel); 
        if (!add_player(game, nick)) 
            addlog("Unable to add % to game.", nick); 
        else
            irc_cmd_msg(session, params[0], game->response); 
    }
    else if (!strcmp(params[1], "!quit")) 
        irc_cmd_quit (session, "Lost Cities bot getting lost...");
    else if (!strcmp(params[1], "!help")) 
        irc_cmd_msg(session, params[0], "there will be help here."); 
}
