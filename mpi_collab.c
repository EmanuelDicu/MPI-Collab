#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "mpi.h"
#include "custom_defines.h"

#include "array_helpers.h"
#include "array_procedures.h"

#include "heartbeat.h"
#include "helper_procedures.h"
#include "log_messages.h"
#include "mpi_wrappers.h"

#include "cluster_helpers.h"
#include "cluster_procedures.h"

#include "workload_helpers.h"
#include "workload_procedures.h"

/* Coordinator main code */
void 
coordinator(int rank, int dim_vec, int err_comm)
{
    int is_master;
    int* neighbor;
    int** cluster;
    int* workload;
    int nr_clusters;
    int parent[COORDINATOR];
    int subtr_size[COORDINATOR];
    MPI_Status status;
    
    neighbor = get_neighbors(rank, err_comm);
    is_master = check_master(neighbor, rank);

    // build parent vector
    if (is_master)
        memset(parent, -1, COORDINATOR * sizeof(*parent));
    else {
        MPI_Recv_Log(parent, COORDINATOR, MPI_INT, MPI_ANY_SOURCE, rank, NEIGHBOR_TAG, MPI_COMM_WORLD, &status);
        parent[rank] = status.MPI_SOURCE;
    }
    broadcast_parent_vector(neighbor, parent, rank);
    rebuild_parent_vector(neighbor, parent, rank);
    if (!is_master)
        MPI_Send_Log(parent, COORDINATOR, MPI_INT, rank, parent[rank], NEIGHBOR_TAG, MPI_COMM_WORLD);
    multicast_parent_vector(parent, rank);
    if (!is_master)
        MPI_Recv_Log(parent, COORDINATOR, MPI_INT, MPI_ANY_SOURCE, rank, NEIGHBOR_TAG, MPI_COMM_WORLD, &status);

    // build topology
    cluster = get_clusters_from_subtree(parent, rank, &nr_clusters);
    if (!is_master) {
        send_clusters(rank, parent[rank], cluster, nr_clusters);
        free_clusters(cluster, nr_clusters);
        cluster = recv_clusters(parent[rank], rank, &nr_clusters);
    } else 
        order_clusters(cluster, nr_clusters);
    print_clusters(rank, cluster, nr_clusters);
    send_topology_to_children(parent, rank, cluster, nr_clusters);

    MPI_Barrier(MPI_COMM_WORLD); // task 1 done

    // build subtree sizes array
    get_subtree_size(parent, rank, subtr_size, cluster, nr_clusters);
    if (!is_master)
        MPI_Send_Log(subtr_size, COORDINATOR, MPI_INT, rank, parent[rank], SUBTRSZ_TAG, MPI_COMM_WORLD);

    // pass workload to workers
    if (rank == GLOBAL_MASTER)
        workload = generate_workload(dim_vec);
    else if (!is_master)
        workload = recv_array(parent[rank], rank, &dim_vec);
    else
        dim_vec = 0, workload = NULL;
    send_workload_to_subtree(workload, dim_vec, subtr_size, cluster, nr_clusters, parent, rank);
    
    // rebuild result from processed workload
    recv_workload_from_subtree(workload, cluster, nr_clusters, parent, rank);
    if (is_master)
        print_workload(workload, dim_vec);
    else
        send_array(rank, parent[rank], workload, dim_vec);

    MPI_Barrier(MPI_COMM_WORLD); // task 2 done

    // free memory
    free_clusters(cluster, nr_clusters);
    free(neighbor);
    if (dim_vec)
        free(workload);
}

/* Woerker main code */
void 
worker(int rank)
{
    int* workload;
    int** cluster;
    int nr_clusters;
    int dim_vec;
    int coordinator_id;
    MPI_Status status;

    // coordinator id
    MPI_Recv_Log(&coordinator_id, 1, MPI_INT, MPI_ANY_SOURCE, rank, NOTIFYWORKER_TAG, MPI_COMM_WORLD, &status);

    // clusters
    cluster = recv_clusters(coordinator_id, rank, &nr_clusters);
    print_clusters(rank, cluster, nr_clusters);    

    MPI_Barrier(MPI_COMM_WORLD); // task 1 done

    // process workload
    workload = recv_array(coordinator_id, rank, &dim_vec);
    for (int i = 0; i < dim_vec; ++i)
        workload[i] *= 5;
    send_array(rank, coordinator_id, workload, dim_vec);

    MPI_Barrier(MPI_COMM_WORLD); // task 2 done

    // free memory
    free_clusters(cluster, nr_clusters);
    if (dim_vec)
        free(workload);
}

int 
main(int argc, char *argv[]) 
{
    int numtasks, rank, len, dim_vec, err_comm;
    char hostname[MPI_MAX_PROCESSOR_NAME];
 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(hostname, &len);

    set_rank(rank); // useful for debug
    
    dim_vec = atoi(argv[1]);
    err_comm = atoi(argv[2]);

    if (rank == GLOBAL_MASTER)
        coordinator(rank, dim_vec, err_comm);
    else if (is_coordinator(rank))
        coordinator(rank, -1, err_comm); // dim_vec known only by master
    else 
        worker(rank);
 
    MPI_Finalize();
}
