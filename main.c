#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>  /* memset, etc */

#include "libircclient.h"

#include "game.h"
#include "irc_game.h"

int main(int argc, char **argv) {
    irc_callbacks_t	callbacks;
    irc_session_t * s;
    unsigned short port = 6667;

    game_board_list_t first_game; 
    game_board_list_t *ctx = &first_game; 

    if (argc != 4) {
        printf ("Usage: %s <server> <nick> <channel>\n", argv[0]);
        return EXIT_FAILURE;
    }

    srandom(time(NULL)); 
    memset (&callbacks, 0, sizeof(callbacks));

    callbacks.event_connect = event_connect;
    callbacks.event_channel = event_channel;
    callbacks.event_join = event_join;
    callbacks.event_nick = event_nick;

    callbacks.event_quit = dump_event;
    callbacks.event_part = dump_event;
    callbacks.event_mode = dump_event;
    callbacks.event_topic = dump_event;
    callbacks.event_kick = dump_event;
    callbacks.event_privmsg = dump_event;
    callbacks.event_notice = dump_event;
    callbacks.event_invite = dump_event;
    callbacks.event_umode = dump_event;

    s = irc_create_session (&callbacks);

    if (!s) {
        printf ("Could not create session\n");
        return EXIT_FAILURE;
    }

    ctx->next = NULL;   /* start with one game in one channel */
    init_game_board(&ctx->game); 
    strncpy(ctx->channel, argv[3], sizeof(ctx->channel)); 
    ctx->nick = argv[2];

    irc_set_ctx(s, ctx);

    // If the port number is specified in the server string, use the port 0 so it gets parsed
    if (strchr(argv[1], ':') != 0)
        port = 0;

    // To handle the "SSL certificate verify failed" from command line we allow passing ## in front 
    // of the server name, and in this case tell libircclient not to verify the cert
    if (argv[1][0] == '#' && argv[1][1] == '#') {
        // Skip the first character as libircclient needs only one # for SSL 
        // support, i.e. #irc.freenode.net
        argv[1]++;
        irc_option_set(s, LIBIRC_OPTION_SSL_NO_VERIFY);
    }

    // Initiate the IRC server connection
    if (irc_connect(s, argv[1], port, 0, argv[2], 0, 0)) {
        printf ("Could not connect: %s\n", irc_strerror (irc_errno(s)));
        return EXIT_FAILURE;
    }

    // and run into forever loop, generating events
    if (irc_run(s)) {
        printf ("Could not connect or I/O error: %s\n", irc_strerror (irc_errno(s)));
        return EXIT_FAILURE;
    }

    deinit_game_board(&ctx->game); 
    return EXIT_SUCCESS; 
}
