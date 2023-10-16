#include "log_messages.h"

#include <stdio.h>

#include "custom_defines.h"

int _rank;

/* Set a global rank variable, used only for debug */
void 
set_rank(int rank)
{
    _rank = rank;
}

/* Log message to terminal */
void 
log_message(char message[])
{
    if (DEBUG)
        printf("P%d: %s\n", _rank, message);
}

/* Log a send message to terminal */
void 
log_send_message(int source, int destination)
{
    if (DEBUG)
        printf("P%d: M(%d,%d)\n", _rank, source, destination);
    else
        printf("M(%d,%d)\n", source, destination);
}

/* Log a receive message to terminal */
void 
log_recv_message(int source, int destination)
{
    if (DEBUG) {
        if (source == -1)
            printf("P%d: R(any,%d)\n", _rank, destination);
        else
            printf("P%d: R(%d,%d)\n", _rank, source, destination);
    }
}
