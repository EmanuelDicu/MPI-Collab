#ifndef CLUSTER_PROCEDURES_H
#define CLUSTER_PROCEDURES_H

void send_clusters(int source, int destination, int* cluster[], int nr_clusters);
int** recv_clusters(int source, int destination, int* ret_nr_clusters);
int** get_clusters_from_subtree(int parent[], int rank, int *ret_nr_clusters);
void send_topology_to_workers(int* cluster[], int nr_clusters, int rank);
void send_topology_to_children(int parent[], int rank, int* cluster[], int nr_clusters);

#endif // CLUSTER_PROCEDURES_H
