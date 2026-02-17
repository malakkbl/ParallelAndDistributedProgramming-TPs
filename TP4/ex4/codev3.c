#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void dmvm(int n, int m,
double *lhs, double *rhs, double *mat)

{
    #pragma omp parallel
    {
        #pragma omp for collapse(2) schedule(static) nowait
        for (int c = 0; c < n; ++c) {
            for (int r = 0; r < m; ++r)
                lhs[r] += mat[r + c*m] * rhs[c];
        }
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

    double start = omp_get_wtime();
    dmvm(n, m, lhs, rhs, mat);
    double end = omp_get_wtime();

    // print result
    printf("lhs:\n");
    for (int r = 0; r < m; ++r)
        printf("lhs[%d] = %.1f\n", r, lhs[r]);

    printf("Execution_time...%lf seconds\n", end - start);

    free(mat);
    free(rhs);
    free(lhs);
    return 0;
}
