#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int rank, num_procs;
    long N;
    double local_sum = 0.0;
    double global_sum = 0.0;
    double pi_approx;
    double start_time, end_time;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    
    // Process 0 reads N
    if (rank == 0) {
        if (argc != 2) {
            printf("Usage: mpirun -np <num_procs> %s <N>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        N = atol(argv[1]);
        if (N <= 0) {
            printf("N must be positive\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    
    // Broadcast N to all processes
    MPI_Bcast(&N, 1, MPI_LONG, 0, MPI_COMM_WORLD);
    
    // Calculate work distribution (handle non-divisible N)
    long base_count = N / num_procs;
    long extra = N % num_procs;
    long local_N = base_count + (rank < extra ? 1 : 0);
    long start_idx = rank * base_count + (rank < extra ? rank : extra);
    
    // Start timing
    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();
    
    // Each process computes its portion
    double x;
    for (long i = start_idx; i < start_idx + local_N; i++) {
        x = (i + 0.5) / N;
        local_sum += 1.0 / (1.0 + x * x);
    }
    
    // Reduce all partial sums to process 0
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    // End timing
    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    
    // Process 0 computes final result
    if (rank == 0) {
        pi_approx = 4.0 * global_sum / N;
        double pi_exact = M_PI;
        double error = fabs(pi_approx - pi_exact);
        double parallel_time = end_time - start_time;
        
        printf("N: %ld\n", N);
        printf("Number of processes: %d\n", num_procs);
        printf("Approximation: %.15f\n", pi_approx);
        printf("Exact value: %.15f\n", pi_exact);
        printf("Error: %.15e\n", error);
        printf("Parallel_Time: %.6f\n", parallel_time);
    }
    
    MPI_Finalize();
    return 0;
}
