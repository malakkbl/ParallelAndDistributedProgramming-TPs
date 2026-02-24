#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double calculate_pi(long N) {
    double sum = 0.0;
    double x;
    
    for (long i = 0; i < N; i++) {
        x = (i + 0.5) / N;
        sum += 1.0 / (1.0 + x * x);
    }
    
    return 4.0 * sum / N;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <N>\n", argv[0]);
        printf("N = number of intervals\n");
        return 1;
    }
    
    long N = atol(argv[1]);
    if (N <= 0) {
        printf("N must be positive\n");
        return 1;
    }
    
    // Start timing
    clock_t start = clock();
    
    double pi_approx = calculate_pi(N);
    
    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    
    double pi_exact = M_PI;
    double error = fabs(pi_approx - pi_exact);
    
    printf("N: %ld\n", N);
    printf("Approximation: %.15f\n", pi_approx);
    printf("Exact value: %.15f\n", pi_exact);
    printf("Error: %.15e\n", error);
    printf("Sequential_Time: %.6f\n", time_taken);
    
    return 0;
}
