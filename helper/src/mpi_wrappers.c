#include "mpi_wrappers.h"

#include "mpi.h"
#include "log_messages.h"
#include "custom_defines.h"

/* Log a send MPI message */
int 
MPI_Send_Log(void* data, int count, MPI_Datatype datatype, 
    int source, int destination, int tag, MPI_Comm communicator)
{
    if (SEND_LOG)
        log_send_message(source, destination);
    return MPI_Send(data, count, datatype, destination, tag, communicator);
}

/* Log a receive MPI message */
int 
MPI_Recv_Log(void* data, int count, MPI_Datatype datatype, 
    int source, int destination, int tag, MPI_Comm communicator, MPI_Status *status)
{
    if (RECV_LOG)
        log_recv_message(source, destination);
    return MPI_Recv(data, count, datatype, source, tag, communicator, status);
}
