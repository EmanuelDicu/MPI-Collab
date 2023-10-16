#ifndef WORKLOAD_PROCEDURES_H
#define WORKLOAD_PROCEDURES_H

void send_workload_to_workers(int workload[], int dim_vec, int cluster[], int rank);
void send_workload_to_subtree(
    int workload[], int dim_vec, int subtr_size[], 
    int* cluster[], int nr_clusters, int parent[], int rank);
int recv_workload_from_workers(int workload[], int cluster[], int rank);
int recv_workload_from_subtree(int workload[], int* cluster[], int nr_clusters, int parent[], int rank);

#endif // WORKLOAD_PROCEDURES_H
