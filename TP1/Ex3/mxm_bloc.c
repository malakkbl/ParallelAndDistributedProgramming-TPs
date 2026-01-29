#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1024   // Matrix size
#ifndef BS
#define BS 32
#endif

double A[N][N], B[N][N], C[N][N];

int min(int a, int b) {
    return a < b ? a : b;
}

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

    // Blocked matrix multiplication
    for (int ii = 0; ii < N; ii += BS)
        for (int kk = 0; kk < N; kk += BS)
            for (int jj = 0; jj < N; jj += BS)
                for (int i = ii; i < min(ii + BS, N); i++)
                    for (int k = kk; k < min(kk + BS, N); k++)
                        for (int j = jj; j < min(jj + BS, N); j++)
                            C[i][j] += A[i][k] * B[k][j];

    clock_gettime(CLOCK_MONOTONIC, &end);

    time_sec = (end.tv_sec - start.tv_sec) +
               (end.tv_nsec - start.tv_nsec) * 1e-9;

    printf("Block size: %d\n", BS);
    printf("Execution time: %f seconds\n", time_sec);

    return 0;
}
