#include "workload_helpers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "custom_defines.h"
#include "log_messages.h"

/* Generate workload array */
int* 
generate_workload(int n)
{
    int i;
    int* workload;

    workload = malloc(n * sizeof(int));
    for (i = 0; i < n; ++i)
        workload[i] = n - i - 1;
    
    return workload;
}

/* Print workload result array to console */
void 
print_workload(int workload[], int dim_vec)
{
    int i;
    char result[MAX_STRINGSIZE];
    
    strcpy(result, "Rezultat: ");
    for (i = 0; i < dim_vec; ++i) 
        sprintf(result + strlen(result), "%d ", workload[i]);
    puts(result);
}

/* Log workload to terminal */
void 
debug_workload(int workload[], int dimvec)
{
    int i;
    char outstr[MAX_STRINGSIZE];

    strcpy(outstr, "Workload: ");
    for (i = 0; i < dimvec; ++i)
        sprintf(outstr + strlen(outstr), "%d ", workload[i]);
    strcat(outstr, "\n");
    
}
