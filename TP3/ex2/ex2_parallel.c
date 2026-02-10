#include <stdio.h>
#include <omp.h>

static long num_steps = 100000;
double step;

int main() {
    int i;
    double x, pi, sum = 0.0;
    double start, end;

    step = 1.0 / (double) num_steps;

    start = omp_get_wtime();

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        double partial_sum = 0.0;

        // Each thread computes its portion
        for (i = id; i < num_steps; i += num_threads) {
            x = (i + 0.5) * step;
            partial_sum += 4.0 / (1.0 + x * x);
        }

        // Critical section to safely add to global sum
        #pragma omp critical
        sum += partial_sum;
    }

    pi = step * sum;

    end = omp_get_wtime();

    printf("PI = %.15f\n", pi);
    printf("Time = %.6f seconds\n", end - start);
    printf("Number of threads = %d\n", omp_get_max_threads());

    return 0;
}
