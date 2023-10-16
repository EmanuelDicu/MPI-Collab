#include "workload_procedures.h"

#include <stdlib.h>
#include <string.h>

#include "cluster_helpers.h"
#include "array_helpers.h"
#include "custom_defines.h"

/* Split workload into equal subarrays and send them to workers */
void
send_workload_to_workers(int workload[], int dim_vec, int cluster[], int rank)
{
    int i;
    int nr_workers;
    int begin_segment;
    int end_segment;

    nr_workers = cluster[1];
    for (i = 0; i < nr_workers; ++i) {
        begin_segment = dim_vec * i / nr_workers;
        end_segment = dim_vec * (i + 1) / nr_workers;
        send_array(rank, cluster[i + 2], workload + begin_segment, end_segment - begin_segment);
    }
}  

/* Split workload into bits proportional with subtree sizes and pass workload recursively */
void
send_workload_to_subtree(
    int workload[], int dim_vec, int subtr_size[], 
    int* cluster[], int nr_clusters,
    int parent[], int rank)
{
    int i;
    int pos_cluster;
    int iter_size;
    int tot_size;
    int begin_segment;
    int end_segment;

    // send a part of the workload to current coordinator's workers
    pos_cluster = get_position_within_cluster(cluster, nr_clusters, rank);
    tot_size = subtr_size[rank];
    iter_size = cluster[pos_cluster][1];
    send_workload_to_workers(workload, dim_vec * iter_size / tot_size, cluster[pos_cluster], rank);

    // send the rest of the workload to sons
    for (i = 0; i < COORDINATOR; ++i) {
        if (parent[i] != rank)
            continue;
        begin_segment = dim_vec * iter_size / tot_size;
        end_segment = dim_vec * (iter_size + subtr_size[i]) / tot_size;
        send_array(rank, i, workload + begin_segment, end_segment - begin_segment);
        iter_size += subtr_size[i];
    }
}

/* Receive processed workload from coordinator's workers */
int 
recv_workload_from_workers(int workload[], int cluster[], int rank)
{
    int i;
    int nr_workers;
    int iterator;
    int* ret_array;
    int ret_length;

    nr_workers = cluster[1];
    iterator = 0;
    for (i = 0; i < nr_workers; ++i) {
        ret_array = recv_array(cluster[i + 2], rank, &ret_length);
        if (ret_length) {
            memcpy(workload + iterator, ret_array, ret_length * sizeof(*ret_array));
            free(ret_array);
        }
        iterator += ret_length;
    }

    return iterator;
}  

/* Receive processed workload from entire subtree */
int
recv_workload_from_subtree(int workload[], int* cluster[], int nr_clusters, int parent[], int rank)
{
    int i;
    int iterator;
    int* ret_array;
    int ret_length;
    int pos_cluster;

    // receive workload from current node's workers 
    pos_cluster = get_position_within_cluster(cluster, nr_clusters, rank);
    iterator = recv_workload_from_workers(workload, cluster[pos_cluster], rank);

    // concatenate workload with bits received from sons
    for (i = 0; i < COORDINATOR; ++i) {
        if (parent[i] != rank)
            continue;
        ret_array = recv_array(i, rank, &ret_length);
        if (ret_length) {
            memcpy(workload + iterator, ret_array, ret_length * sizeof(*ret_array));
            free(ret_array);
        }
        iterator += ret_length;
    }

    return iterator;
}
