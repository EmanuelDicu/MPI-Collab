#ifndef ARRAY_HELPERS_H
#define ARRAY_HELPERS_H

void send_array(int source, int destination, int* data, int length);
int* recv_array(int source, int destination, int *ret_length);

#endif // ARRAY_HELPERS_H
