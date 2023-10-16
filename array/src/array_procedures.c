#include "array_procedures.h"

#include <string.h>

#include "custom_defines.h"
#include "mpi.h"
#include "mpi_wrappers.h"

/* Send parent vector to all neighbors of a node, with the exception of the parent node */
void 
broadcast_parent_vector(int neighbor[], int parent[], int rank)
{
    int i;

    for (i = 0; i < 2; ++i) {
        if (neighbor[i] == -1 || neighbor[i] == parent[rank])
            continue;
        MPI_Send_Log(parent, COORDINATOR, MPI_INT, rank, neighbor[i], NEIGHBOR_TAG, MPI_COMM_WORLD);   
    }
}

/* Send parent vector only to his sons */
void 
multicast_parent_vector(int parent[], int rank)
{
    int i;

    for (i = 0; i < COORDINATOR; ++i)
        if (parent[i] == rank)
            MPI_Send_Log(parent, COORDINATOR, MPI_INT, rank, i, NEIGHBOR_TAG, MPI_COMM_WORLD);
}

/* Receive bits of the parent vector from adjacent nodes different than the parent and combine them */
void 
rebuild_parent_vector(int neighbor[], int parent[], int rank)
{
    int i;
    int parent_recv[COORDINATOR];
    MPI_Status status;
    
    for (i = 0; i < 2; ++i) {
        if (neighbor[i] == -1 || neighbor[i] == parent[rank])
            continue;
        MPI_Recv_Log(parent_recv, COORDINATOR, MPI_INT, neighbor[i], rank, NEIGHBOR_TAG, MPI_COMM_WORLD, &status);

        // combine parent array with received parent bits
        for (int j = 0; j < COORDINATOR; ++j)
            if (parent_recv[j] != -1)
                parent[j] = parent_recv[j];
    }
}

/* Get subtree sizes from sons and from the node's cluster and combine them */
int 
get_subtree_size(
    int parent[], int rank, int subtr_size[], 
    int* cluster[], int nr_clusters)
{
    int i, j;
    int subtr_size_recv[COORDINATOR];
    MPI_Status status;

    memset(subtr_size, 0, COORDINATOR * sizeof(*subtr_size));
    
    // get subtree sizes from sons
    for (i = 0; i < COORDINATOR; ++i) {
        if (parent[i] != rank) 
            continue;
        
        MPI_Recv_Log(subtr_size_recv, COORDINATOR, MPI_INT, i, rank, SUBTRSZ_TAG, MPI_COMM_WORLD, &status);
        for (j = 0; j < COORDINATOR; ++j) 
            if (subtr_size_recv[j] > 0) 
                subtr_size[j] = subtr_size_recv[j];
    }

    // get also the current coordinator's subtree size
    for (i = 0; i < nr_clusters; ++i)
        if (cluster[i][0] == rank)
            subtr_size[rank] = cluster[i][1];

    // add son's subtree sizes to current subtree size
    for (i = 0; i < COORDINATOR; ++i)
        if (parent[i] == rank)
            subtr_size[rank] += subtr_size[i];

    return subtr_size[rank];
}
