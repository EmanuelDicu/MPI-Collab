#ifndef ARRAY_PROCEDURES_H
#define ARRAY_PROCEDURES_H

void broadcast_parent_vector(int neighbor[], int parent[], int rank);
void multicast_parent_vector(int parent[], int rank);
void rebuild_parent_vector(int neighbor[], int parent[], int rank);

int get_subtree_size(
    int parent[], int rank, int subtr_size[], 
    int* cluster[], int nr_clusters);

#endif // ARRAY_PROCEDURES_H
