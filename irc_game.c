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
 * handle commands from channel
 ******************************************/
static int handle_quit(irc_session_t * session, const char * event, const char * origin,
        const char ** params, unsigned int count)
{
    irc_cmd_quit (session, "Lost Cities bot getting lost...");
    return 1;
}

static int handle_help(irc_session_t * session, const char * event, const char * origin,
        const char ** params, unsigned int count)
{
    irc_cmd_msg(session, params[0], "there will be help here."); 
    return 1;
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
    irc_cmd_user_mode (session, "+i");
    irc_cmd_msg (session, params[0], "Let's get this exploring party started!");
}
void event_nick(irc_session_t * session, const char * event, const char * origin,
        const char ** params, unsigned int count)
{
    dump_event(session, event, origin, params, count); 
    /* GTL - check if a game player changed his/her nick and update the game if needed. */
}

void event_channel(irc_session_t * session, const char * event, const char * origin,
        const char ** params, unsigned int count) {
    char nickbuf[128];
    dump_event(session, event, origin, params, count); 

    if (count != 2)
        return;

    if (!origin)
        return;

    if (!params[1] == '!') 
        return;

    irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));

    struct {
        const char *cmd;
        int (*action)(irc_session_t *, const char*, const char*, const char**, unsigned int); 
    } cmd_map[] = { 
        { "!quit", handle_quit }, 
        { "!help", handle_help }
    };

    for (int i = 0; i < sizeof(cmd_map)/sizeof(cmd_map[0]); i++) {
        if (!strcmp(params[1], cmd_map[i].cmd)) {
            addlog("Executing action %s\n", cmd_map[i].cmd); 
            if (!cmd_map[i].action(session, event, origin, params, count)) 
                addlog("Error executing command.\n");
        }
    }
}

