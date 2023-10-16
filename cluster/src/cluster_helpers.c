#include "cluster_helpers.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mpi_wrappers.h"
#include "mpi.h"
#include "custom_defines.h"

/* Comparison function for qsort used to order coordinators */
int
cmp_coordinator(const void* a, const void* b)
{
    return **(int**)a - **(int**)b;
}

/* Comparison function for qsort used to order workers */
int 
cmp_workers(const void* a, const void* b)
{
    return *(int*)a - *(int*)b;
}

/* Order clusters by coordinator and then by workers */
void
order_clusters(int* cluster[], int nr_clusters)
{
    int i;

    qsort(cluster, nr_clusters, sizeof(*cluster), cmp_coordinator);
    for (i = 0; i < nr_clusters; ++i) {
        int nr_workers = cluster[i][1];
        qsort(cluster[i] + 2, nr_workers, sizeof(*(cluster[i] + 2)), cmp_workers);
    }
}

/* Concatenate two arrays of clusters */
int** 
concat_clusters(int** cluster1, int nr_clusters1, int** cluster2, int nr_clusters2)
{
    int i;
    cluster1 = realloc(cluster1, (nr_clusters1 + nr_clusters2) * sizeof(*cluster1));
    for (i = nr_clusters1; i < nr_clusters1 + nr_clusters2; ++i)
        cluster1[i] = cluster2[i - nr_clusters1];
    free(cluster2);

    return cluster1;
}

/* Given a rank, find the index of the coordinator with the same rank within the cluster */
int 
get_position_within_cluster(int* cluster[], int nr_clusters, int rank)
{
    int i;

    for (i = 0; i < nr_clusters; ++i) 
        if (cluster[i][0] == rank)  
            return i;

    return -1;
}

/* Read a cluster from an input file (and notify all the workers of their coordinator) */
int* 
get_cluster(int rank)
{
    FILE *fd;
    int i;
    int nr_workers;
    int* cluster;
    int worker_id;
    char filename[MAX_FILESIZE];

    sprintf(filename, "cluster%d.txt", rank);
    fd = fopen(filename, "r");

    fscanf(fd, "%d\n", &nr_workers);
    cluster = malloc((nr_workers + 2) * sizeof(*cluster));
    cluster[0] = rank;
    cluster[1] = nr_workers;
    for (i = 0; i < nr_workers; ++i) {
        fscanf(fd, "%d\n", &worker_id);
        cluster[i + 2] = worker_id;
        MPI_Send_Log(&rank, 1, MPI_INT, rank, worker_id, NOTIFYWORKER_TAG, MPI_COMM_WORLD);
    }
    
    fclose(fd);
    return cluster;
}

/* Print a list of clusters to console */
void 
print_clusters(int rank, int* cluster[], int nr_clusters)
{
    int i, j;
    int coordinator;
    int nr_workers;
    char topology[MAX_STRINGSIZE];

    sprintf(topology, "%d -> ", rank);
    for (i = 0; i < nr_clusters; ++i) {
        coordinator = cluster[i][0];
        nr_workers = cluster[i][1];

        sprintf(topology + strlen(topology), "%d:", coordinator);
        for (j = 0; j < nr_workers; ++j) {
            sprintf(topology + strlen(topology), "%d", cluster[i][j + 2]);
            if (j + 1 < nr_workers)
                strcat(topology, ",");
            else 
                strcat(topology, " ");
        }
    }
    puts(topology);
}

/* Free cluster memory */
void
free_clusters(int* cluster[], int nr_clusters)
{
    int i;

    for (i = 0; i < nr_clusters; ++i)
        free(cluster[i]);
    free(cluster);
}
