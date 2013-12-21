#ifndef GAME_COMMANDS_H
#define GAME_COMMANDS_H

/*
 * handle game commands from user. 
 */
typedef struct {
    const char *cmd;        /* command from the user */
    const char *help;       /* help assciated with the command */
                            /* command callback */
    int (*func)(irc_session_t * session, const char * event, const char * origin,
        const char ** params, unsigned int count);
} game_command; 

extern game_command game_commands[]; 
extern size_t num_game_commands; 

#endif /* GAME_COMMANDS_H */
