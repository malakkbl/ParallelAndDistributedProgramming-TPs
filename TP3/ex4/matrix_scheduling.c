#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <num_threads> <schedule_type> <chunk_size>\n", argv[0]);
        printf("Example: %s 8 static 50\n", argv[0]);
        printf("schedule_type: static, dynamic, guided\n");
        return 1;
    }
    
    int num_threads = atoi(argv[1]);
    char *schedule_type = argv[2];
    int chunk_size = atoi(argv[3]);
    
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
    
    // Apply scheduling based on input
    if (strcmp(schedule_type, "static") == 0) {
        #pragma omp parallel for collapse(2) private(i, j, k) schedule(static, chunk_size)
        for (i = 0; i < m; i++) {
            for (j = 0; j < m; j++) {
                for (k = 0; k < n; k++) {
                    c[i * m + j] += a[i * n + k] * b[k * m + j];
                }
            }
        }
    } else if (strcmp(schedule_type, "dynamic") == 0) {
        #pragma omp parallel for collapse(2) private(i, j, k) schedule(dynamic, chunk_size)
        for (i = 0; i < m; i++) {
            for (j = 0; j < m; j++) {
                for (k = 0; k < n; k++) {
                    c[i * m + j] += a[i * n + k] * b[k * m + j];
                }
            }
        }
    } else if (strcmp(schedule_type, "guided") == 0) {
        #pragma omp parallel for collapse(2) private(i, j, k) schedule(guided, chunk_size)
        for (i = 0; i < m; i++) {
            for (j = 0; j < m; j++) {
                for (k = 0; k < n; k++) {
                    c[i * m + j] += a[i * n + k] * b[k * m + j];
                }
            }
        }
    } else {
        printf("Error: Invalid schedule type '%s'\n", schedule_type);
        free(a); free(b); free(c);
        return 1;
    }
    
    end = omp_get_wtime();
    
    // Output format: threads,schedule,chunk_size,time
    printf("%d,%s,%d,%.6f\n", num_threads, schedule_type, chunk_size, end - start);
    
    free(a);
    free(b);
    free(c);
    
    return 0;
}
