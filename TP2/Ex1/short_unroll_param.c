#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000000

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s <unrolling_factor>\n", argv[0]);
        return 1;
    }

    int U = atoi(argv[1]);

    short *a = malloc(N * sizeof(short));
    int sum = 0;
    double start, end;

    for (int i = 0; i < N; i++)
        a[i] = 1;

    start = (double)clock() / CLOCKS_PER_SEC;

    switch (U) {

        case 1:
            for (int i = 0; i < N; i++)
                sum += a[i];
            break;

        case 2:
            for (int i = 0; i < N; i += 2)
                sum += a[i] + a[i+1];
            break;

        case 4:
            for (int i = 0; i < N; i += 4)
                sum += a[i] + a[i+1] + a[i+2] + a[i+3];
            break;

        case 8:
            for (int i = 0; i < N; i += 8)
                sum += a[i] + a[i+1] + a[i+2] + a[i+3]
                     + a[i+4] + a[i+5] + a[i+6] + a[i+7];
            break;

        case 16:
            for (int i = 0; i < N; i += 16)
                sum += a[i]  + a[i+1]  + a[i+2]  + a[i+3]
                     + a[i+4]  + a[i+5]  + a[i+6]  + a[i+7]
                     + a[i+8]  + a[i+9]  + a[i+10] + a[i+11]
                     + a[i+12] + a[i+13] + a[i+14] + a[i+15];
            break;

        case 32:
            for (int i = 0; i < N; i += 32)
                sum += a[i]  + a[i+1]  + a[i+2]  + a[i+3]
                     + a[i+4]  + a[i+5]  + a[i+6]  + a[i+7]
                     + a[i+8]  + a[i+9]  + a[i+10] + a[i+11]
                     + a[i+12] + a[i+13] + a[i+14] + a[i+15]
                     + a[i+16] + a[i+17] + a[i+18] + a[i+19]
                     + a[i+20] + a[i+21] + a[i+22] + a[i+23]
                     + a[i+24] + a[i+25] + a[i+26] + a[i+27]
                     + a[i+28] + a[i+29] + a[i+30] + a[i+31];
            break;

        default:
            printf("Unsupported unrolling factor: %d\n", U);
            free(a);
            return 1;
    }

    end = (double)clock() / CLOCKS_PER_SEC;

    printf("SHORT | U=%d | Sum = %d | Time = %f ms\n",
           U, sum, (end - start) * 1000);

    free(a);
    return 0;
}
