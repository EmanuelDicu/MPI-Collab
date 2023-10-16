#ifndef CLUSTER_HELPERS_H
#define CLUSTER_HELPERS_H

int cmp_coordinator(const void* a, const void* b);
int cmp_workers(const void* a, const void* b);
void order_clusters(int* cluster[], int nr_clusters);
int** concat_clusters(int** cluster1, int nr_clusters1, int** cluster2, int nr_clusters2);
int get_position_within_cluster(int* cluster[], int nr_clusters, int rank);
int* get_cluster(int rank);
void order_clusters(int* cluster[], int nr_clusters);
void print_clusters(int rank, int* cluster[], int nr_clusters);
void free_clusters(int* cluster[], int nr_clusters);

#endif // CLUSTER_HELPERS_H
