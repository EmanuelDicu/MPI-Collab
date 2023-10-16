#include "array_helpers.h"

#include <stdlib.h>

#include "mpi.h"
#include "custom_defines.h"
#include "mpi_wrappers.h"

/* Send array of undefined length using MPI */
void 
send_array(int source, int destination, int* data, int length)
{
    MPI_Send_Log(&length, 1, MPI_INT, source, destination, ARRAY_TAG, MPI_COMM_WORLD);
    if (length)
        MPI_Send_Log(data, length, MPI_INT, source, destination, ARRAY_TAG, MPI_COMM_WORLD);
}

/* Receive array of undefined length using MPI */
int* 
recv_array(int source, int destination, int *ret_length)
{
    int length;
    int* data = NULL;
    MPI_Status status;

    MPI_Recv_Log(&length, 1, MPI_INT, source, destination, ARRAY_TAG, MPI_COMM_WORLD, &status);
    if (length) {
        data = malloc(length * sizeof(*data));
        MPI_Recv_Log(data, length, MPI_INT, source, destination, ARRAY_TAG, MPI_COMM_WORLD, &status);
    }

    *ret_length = length;
    return data;
}
