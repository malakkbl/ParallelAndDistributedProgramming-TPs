#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }
    
    int num_threads = atoi(argv[1]);
    omp_set_num_threads(num_threads);
    
    int m = 1000;
    int n = 1000;
    double *a, *b, *c;
    int i, j, k;
    double start, end;
    
    // Allocate memory
    a = (double *)malloc(m * n * sizeof(double));
    b = (double *)malloc(n * m * sizeof(double));
    c = (double *)malloc(m * m * sizeof(double));
    
    // Initialize matrices
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            a[i * n + j] = (i + 1) + (j + 1);
        }
    }
    
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            b[i * m + j] = (i + 1) - (j + 1);
        }
    }
    
    for (i = 0; i < m; i++) {
        for (j = 0; j < m; j++) {
            c[i * m + j] = 0;
        }
    }
    
    start = omp_get_wtime();
    
    // Parallel matrix multiplication with collapse
    #pragma omp parallel for collapse(2) private(i, j, k)
    for (i = 0; i < m; i++) {
        for (j = 0; j < m; j++) {
            for (k = 0; k < n; k++) {
                c[i * m + j] += a[i * n + k] * b[k * m + j];
            }
        }
    }
    
    end = omp_get_wtime();
    
    // Output format: threads,time
    printf("%d,%.6f\n", num_threads, end - start);
    
    free(a);
    free(b);
    free(c);
    
    return 0;
}
