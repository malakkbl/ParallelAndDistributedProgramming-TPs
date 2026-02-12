#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    int m = 1000;
    int n = 1000;
    int i, j, k;
    double start, end;
    
    // Allocate memory dynamically
    double *a = (double *)malloc(m * n * sizeof(double));
    double *b = (double *)malloc(n * m * sizeof(double));
    double *c = (double *)malloc(m * m * sizeof(double));
    
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
    
    // Matrix multiplication
    for (i = 0; i < m; i++) {
        for (j = 0; j < m; j++) {
            for (k = 0; k < n; k++) {
                c[i * m + j] += a[i * n + k] * b[k * m + j];
            }
        }
    }
    
    end = omp_get_wtime();
    
    printf("Serial Matrix Multiplication\n");
    printf("Matrix dimensions: A[%dx%d] * B[%dx%d] = C[%dx%d]\n", m, n, n, m, m, m);
    printf("Time: %.6f seconds\n", end - start);
    printf("Sample result c[0][0] = %.2f\n", c[0]);
    
    free(a);
    free(b);
    free(c);
    
    return 0;
}
