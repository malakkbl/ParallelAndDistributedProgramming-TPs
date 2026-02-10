#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        printf("Hello from the rank %d thread\n", thread_id);

        // Only master thread prints the total
        #pragma omp master
        printf("Parallel execution is initialized with %d threads\n", num_threads);
    }

    return 0;
}
