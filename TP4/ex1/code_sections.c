#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define N 1000000

int main() {
    
    double *A = malloc(N * sizeof(double));
    if (A == NULL) {
        fprintf(stderr, "Allocation failed\n");
        return 1;
    }
    
    double sum = 0.0;
    double mean = 0.0;
    double stddev = 0.0;
    double max;
    
    // Initialization
    srand(0);
    for (int i = 0; i < N; i++)
        A[i] = (double)rand() / RAND_MAX;
    
    // Parallel sections for independent computations
    #pragma omp parallel sections
    {
        // Section 1: Compute sum
        #pragma omp section
        {
            sum = 0.0;
            for (int i = 0; i < N; i++)
                sum += A[i];
        }
        
        // Section 2: Compute max
        #pragma omp section
        {
            max = A[0];
            for (int i = 0; i < N; i++)
                if (A[i] > max)
                    max = A[i];
        }
        
        // Section 3: Compute standard deviation
        #pragma omp section
        {
            stddev = 0.0;
            for (int i = 0; i < N; i++)
                stddev += (A[i] - mean) * (A[i] - mean);
        }
    }
    
    // Compute mean (depends on sum from Section 1)
    mean = sum / N;
    
    // Complete standard deviation calculation (depends on mean)
    stddev = sqrt(stddev / N);
    
    // Print results
    printf("Sum.........%f\n", sum);
    printf("Max.........%f\n", max);
    printf("Std.Dev.....%f\n", stddev);
    
    free(A);
    return 0;
}
