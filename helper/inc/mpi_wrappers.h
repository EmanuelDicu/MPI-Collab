#ifndef MPI_WRAPPERS_H
#define MPI_WRAPPERS_H

#include "mpi.h"

int MPI_Send_Log(void* data, int count, MPI_Datatype datatype, 
    int source, int destination, int tag, MPI_Comm communicator);
int MPI_Recv_Log(void* data, int count, MPI_Datatype datatype, 
    int source, int destination, int tag, MPI_Comm communicator, MPI_Status *status);

#endif // MPI_WRAPPERS_H
