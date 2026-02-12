#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

#ifndef VAL_N
#define VAL_N 120
#endif

#ifndef VAL_D
#define VAL_D 80
#endif

void random_number(double* array, int size) {
    for (int i = 0; i < size; ++i) {
        array[i] = (double)rand() / (double)(RAND_MAX - 1);
    }
}

int main() {
    int n = VAL_N, diag = VAL_D;
    int i, j, iteration = 0;
    double norme;

    double *a = (double*)malloc(n * n * sizeof(double));
    double *x = (double*)malloc(n * sizeof(double));
    double *b = (double*)malloc(n * sizeof(double));
    double *x_courant = (double*)malloc(n * sizeof(double));

    if (!a || !x || !b || !x_courant) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    struct timeval t_elapsed_0, t_elapsed_1;
    double t_elapsed;
    double t_cpu_0, t_cpu_1, t_cpu;

    srand(421);
    random_number(b, n);
    random_number(a, n * n);

    for (i = 0; i < n; ++i) {
        a[i * n + i] += diag;
    }

    for (i = 0; i < n; ++i) {
        x[i] = 1.0;
    }

    gettimeofday(&t_elapsed_0, NULL);
    t_cpu_0 = omp_get_wtime();

    while (1) {
        iteration++;
        
        for (i = 0; i < n; ++i) {
            x_courant[i] = 0;
            for (j = 0; j < i; ++j) {
                x_courant[i] += a[i * n + j] * x[j];
            }
            for (j = i + 1; j < n; ++j) {
                x_courant[i] += a[i * n + j] * x[j];
            }
            x_courant[i] = (b[i] - x_courant[i]) / a[i * n + i];
        }
        
        double nbnorm = 0;
        for (i = 0; i < n; ++i) {
            double curr = fabs(x[i] - x_courant[i]);
            if (curr > nbnorm)
                nbnorm = curr;
        }
        norme = nbnorm / n;
        
        if ((norme <= DBL_EPSILON) || (iteration >= n)) break;
        
        memcpy(x, x_courant, n * sizeof(double));
    }

    gettimeofday(&t_elapsed_1, NULL);
    t_cpu_1 = omp_get_wtime();
    t_elapsed = (t_elapsed_1.tv_sec - t_elapsed_0.tv_sec) * 1000.0;
    t_elapsed += (t_elapsed_1.tv_usec - t_elapsed_0.tv_usec) / 1000.0;
    t_cpu = (t_cpu_1 - t_cpu_0) * 1e3;

    fprintf(stdout, "Iterations.........%d\n", iteration);
    fprintf(stdout, "Norme..............%10.3e\n", norme);
    fprintf(stdout, "Elapsed_time.......%10.3e ms\n", t_elapsed);
    fprintf(stdout, "CPU_time...........%10.3e ms\n", t_cpu);
    
    free(a); free(x); free(b); free(x_courant);
    return EXIT_SUCCESS;
}
