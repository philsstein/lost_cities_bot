#include <stdio.h>
#include <stdarg.h>
#include "log.h"

int addlog (const char * fmt, ...) 
{
    FILE * fp;
    char buf[1024];
    va_list va_alist;
    int retval;

    va_start(va_alist, fmt);
#if defined (_WIN32)
    retval = _vsnprintf (buf, sizeof(buf), fmt, va_alist);
#else
    retval = vsnprintf (buf, sizeof(buf), fmt, va_alist);
#endif
    va_end (va_alist);

    fprintf(stdout, "%s\n", buf);

    if ( (fp = fopen ("irc_game.log", "ab")) != 0 ) {
        fprintf (fp, "%s\n", buf);
        fclose (fp);
    }
    return retval;
}

