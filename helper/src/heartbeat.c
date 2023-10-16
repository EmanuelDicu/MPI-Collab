#include "heartbeat.h"

#include "mpi_wrappers.h"
#include "mpi.h"
#include "custom_defines.h"

/* Heartbeat algorithm for determining the master */
int
check_master(int neighbor[], int rank)
{
    int i;
    int step;
    int master;
    int new_master;
    MPI_Status status;

    master = rank;
    for (step = 0; step < COORDINATOR; ++step) {
        for (i = 0; i < 2; ++i) {
            if (neighbor[i] == -1)
                continue;
            MPI_Send_Log(&master, 1, MPI_INT, rank, neighbor[i], GETMASTER_TAG, MPI_COMM_WORLD);
            MPI_Recv_Log(&new_master, 1, MPI_INT, MPI_ANY_SOURCE, rank, GETMASTER_TAG, MPI_COMM_WORLD, &status);
            if (master > new_master)
                master = new_master;
        }
    }

    return (master == rank);
}
