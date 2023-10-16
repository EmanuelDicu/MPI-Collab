#ifndef HELPER_PROCEDURES_H
#define HELPER_PROCEDURES_H

int is_coordinator(int rank);
int is_worker(int rank);
int* get_neighbors(int rank, int err_comm);

#endif // HELPER_PROCEDURES_H
