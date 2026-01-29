#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1024

double A[N][N], B[N][N], C[N][N];

int main() {
    struct timespec start, end;
    double time_sec;

    // Initialization
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            A[i][j] = 1.0;
            B[i][j] = 1.0;
            C[i][j] = 0.0;
        }

    clock_gettime(CLOCK_MONOTONIC, &start);

    // Optimized matrix multiplication (i-k-j)
    for (int i = 0; i < N; i++)
        for (int k = 0; k < N; k++)
            for (int j = 0; j < N; j++)
                C[i][j] += A[i][k] * B[k][j];

    clock_gettime(CLOCK_MONOTONIC, &end);

    time_sec = (end.tv_sec - start.tv_sec) +
               (end.tv_nsec - start.tv_nsec) * 1e-9;

    printf("Execution time (ikj): %f seconds\n", time_sec);

    return 0;
}
