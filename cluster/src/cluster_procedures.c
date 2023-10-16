#include "cluster_procedures.h"

#include <stdlib.h>

#include "cluster_helpers.h"
#include "array_helpers.h"
#include "mpi_wrappers.h"
#include "mpi.h"
#include "custom_defines.h"

/* Send an array of clusters of undefined length using MPI */
void 
send_clusters(int source, int destination, int* cluster[], int nr_clusters)
{
    int i;
    int nr_workers;

    MPI_Send_Log(&nr_clusters, 1, MPI_INT, source, destination, CLUSTER_TAG, MPI_COMM_WORLD);
    for (i = 0; i < nr_clusters; ++i) {
        nr_workers = cluster[i][1];
        send_array(source, destination, cluster[i], nr_workers + 2);
    }
}

/* Receive an array of clusters of undefined length using MPI */
int** 
recv_clusters(int source, int destination, int* ret_nr_clusters)
{
    int i;
    int arr_len;
    int nr_clusters;
    int** cluster;
    MPI_Status status;

    MPI_Recv_Log(&nr_clusters, 1, MPI_INT, source, destination, CLUSTER_TAG, MPI_COMM_WORLD, &status);
    cluster = malloc(nr_clusters * sizeof(*cluster));
    for (i = 0; i < nr_clusters; ++i)
        cluster[i] = recv_array(source, destination, &arr_len);
        
    *ret_nr_clusters = nr_clusters;
    return cluster;
}

/* Combine node cluster and subtree clusters */
int** 
get_clusters_from_subtree(int parent[], int rank, int *ret_nr_clusters)
{
    int i;
    int **cluster, **new_cluster;
    int nr_clusters, nr_new_clusters;

    // read current node cluster from file
    nr_clusters = 1;
    cluster = malloc(nr_clusters * sizeof(*cluster));
    cluster[0] = get_cluster(rank);

    // concat with clusters from subtree
    for (i = 0; i < COORDINATOR; ++i) {
        if (parent[i] != rank) 
            continue;
        new_cluster = recv_clusters(i, rank, &nr_new_clusters);
        cluster = concat_clusters(cluster, nr_clusters, new_cluster, nr_new_clusters);
        nr_clusters += nr_new_clusters;
    }

    *ret_nr_clusters = nr_clusters;
    return cluster;
}

/* Send entire topology to coordinator's workers */
void
send_topology_to_workers(int* cluster[], int nr_clusters, int rank)
{
    int i;
    int pos_cluster;
    int nr_workers;

    pos_cluster = get_position_within_cluster(cluster, nr_clusters, rank);
    nr_workers = cluster[pos_cluster][1];
    for (i = 0; i < nr_workers; ++i)
        send_clusters(rank, cluster[pos_cluster][i + 2], cluster, nr_clusters);
}

/* Send entire topology to both workers and subtree recursively */
void 
send_topology_to_children(int parent[], int rank, int* cluster[], int nr_clusters)
{
    int i;

    send_topology_to_workers(cluster, nr_clusters, rank);
    for (i = 0; i < COORDINATOR; ++i)
        if (parent[i] == rank)
            send_clusters(rank, i, cluster, nr_clusters);
}
