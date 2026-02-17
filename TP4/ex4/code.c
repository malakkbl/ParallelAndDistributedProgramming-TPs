#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void dmvm(int n, int m,
double *lhs, double *rhs, double *mat)
{
    for (int c = 0; c < n; ++c) {
        int offset = m * c;
        for (int r = 0; r < m; ++r)
            lhs[r] += mat[r + offset] * rhs[c];
    }
}

int main(void)
{
    const int n = 40000; // columns
    const int m = 600; // rows

    double *mat = malloc(n * m * sizeof(double));
    double *rhs = malloc(n * sizeof(double));
    double *lhs = malloc(m * sizeof(double));

    // initialization
    for (int c = 0; c < n; ++c) {
        rhs[c] = 1.0;
        for (int r = 0; r < m; ++r)
            mat[r + c*m] = 1.0;
    }

    for (int r = 0; r < m; ++r)
        lhs[r] = 0.0;

    dmvm(n, m, lhs, rhs, mat);

    // print result
    printf("lhs:\n");
    for (int r = 0; r < m; ++r)
        printf("lhs[%d] = %.1f\n", r, lhs[r]);


    free(mat);
    free(rhs);
    free(lhs);
    return 0;
}
