#include "helper_procedures.h"

#include <stdlib.h>

#include "custom_defines.h"

/* Check if node is coordinator */
int 
is_coordinator(int rank)
{
    return rank < COORDINATOR;
}

/* Check if node is worker */
int 
is_worker(int rank)
{
    return !is_coordinator(rank);
}

/* Get neighbors of current node, as explained in the statement */
int* 
get_neighbors(int rank, int err_comm)
{
    int* neighbor;

    neighbor = malloc(2 * sizeof(*neighbor));
    switch (rank) {
    case 0:
        neighbor[0] = (err_comm >= 1) ? -1 : 1;
        neighbor[1] = 3;
        break;
    case 1:
        neighbor[0] = (err_comm >= 2) ? -1 : 2;
        neighbor[1] = (err_comm >= 1) ? -1 : 0;
        break;
    case 2:
        neighbor[0] = 3;
        neighbor[1] = (err_comm >= 2) ? -1 : 1;
        break;
    case 3:
        neighbor[0] = 0;
        neighbor[1] = 2;
        break;
    }

    return neighbor;
}
