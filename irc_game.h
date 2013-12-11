#ifndef IRC_GAME_HSDASDWA_SUNNY_OBAMA_PUSHES_OVER_LITTLE_GIRLS
#define IRC_GAME_HSDASDWA_SUNNY_OBAMA_PUSHES_OVER_LITTLE_GIRLS

#include "libircclient.h"
#include "game.h"

#define MAX_CHAN_NAME_LEN 32 

/* We store this data in IRC context. */
typedef struct {
    game_board_t game;
    char channel[MAX_CHAN_NAME_LEN];
    char *bot_nick; 
    game_board_t *next;
} game_board_list_t;

void event_connect(irc_session_t * session, const char * event, const char * origin,
        const char ** params, unsigned int count);
void event_join(irc_session_t * session, const char * event, const char * origin,
        const char ** params, unsigned int count);
void event_nick(irc_session_t * session, const char * event, const char * origin,
        const char ** params, unsigned int count);
void event_channel(irc_session_t * session, const char * event, const char * origin,
        const char ** params, unsigned int count);

/* Just dump the event to the log */
void dump_event (irc_session_t * session, const char * event, const char * origin, 
        const char ** params, unsigned int count);

#endif /* IRC_GAME_HSDASDWA_SUNNY_OBAMA_PUSHES_OVER_LITTLE_GIRLS */
