#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

void matrixVectorMult(double* A, double* b, double* x, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        x[i] = 0.0;
        for (int j = 0; j < cols; ++j) {
            x[i] += A[i * cols + j] * b[j];
        }
    }
}

int main(int argc, char* argv[]) {
    int rank, num_procs;
    int size;
    double *A = NULL;
    double *b = NULL;
    double *x_parallel = NULL;
    double *local_A = NULL;
    double *local_x = NULL;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    
    // Process 0 reads arguments and initializes data
    if (rank == 0) {
        if (argc != 2) {
            printf("Usage: mpirun -np <num_procs> %s <matrix_size>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        size = atoi(argv[1]);
        if (size <= 0) {
            printf("Matrix size must be positive.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    
    // Broadcast size to all processes
    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Calculate local rows for each process
    int base_rows = size / num_procs;
    int extra_rows = size % num_procs;
    int local_rows = base_rows + (rank < extra_rows ? 1 : 0);
    
    // Calculate start row for this process
    int start_row = rank * base_rows + (rank < extra_rows ? rank : extra_rows);
    
    // Allocate local arrays
    local_A = malloc(local_rows * size * sizeof(double));
    local_x = malloc(local_rows * sizeof(double));
    b = malloc(size * sizeof(double));
    
    // Process 0 initializes matrix A and vector b
    if (rank == 0) {
        A = malloc(size * size * sizeof(double));
        x_parallel = malloc(size * sizeof(double));
        
        if (!A || !x_parallel) {
            printf("Memory allocation failed.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        
        srand48(42);
        
        // Initialize matrix A with zeros
        for (int i = 0; i < size * size; ++i)
            A[i] = 0.0;
        
        // Fill A[0][:100] with random values
        int limit = (size < 100) ? size : 100;
        for (int j = 0; j < limit; ++j)
            A[0 * size + j] = drand48();
        
        // Copy A[0][:100] into A[1][100:200] if possible
        if (size > 1 && size > 100) {
            int copy_len = (size - 100 < 100) ? (size - 100) : 100;
            for (int j = 0; j < copy_len; ++j)
                A[1 * size + (100 + j)] = A[0 * size + j];
        }
        
        // Set diagonal
        for (int i = 0; i < size; ++i)
            A[i * size + i] = drand48();
        
        // Fill vector b
        for (int i = 0; i < size; ++i)
            b[i] = drand48();
    }
    
    // Broadcast vector b to all processes
    MPI_Bcast(b, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // Prepare sendcounts and displacements for Scatterv
    int *sendcounts = NULL;
    int *displs = NULL;
    int *recvcounts = NULL;
    int *recvdispls = NULL;
    
    if (rank == 0) {
        sendcounts = malloc(num_procs * sizeof(int));
        displs = malloc(num_procs * sizeof(int));
        recvcounts = malloc(num_procs * sizeof(int));
        recvdispls = malloc(num_procs * sizeof(int));
        
        int offset = 0;
        int row_offset = 0;
        for (int i = 0; i < num_procs; i++) {
            int rows_for_proc = base_rows + (i < extra_rows ? 1 : 0);
            sendcounts[i] = rows_for_proc * size;
            displs[i] = offset;
            recvcounts[i] = rows_for_proc;
            recvdispls[i] = row_offset;
            offset += sendcounts[i];
            row_offset += rows_for_proc;
        }
    }
    
    // Start timing
    double start_time = MPI_Wtime();
    
    // Scatter matrix rows to all processes
    MPI_Scatterv(A, sendcounts, displs, MPI_DOUBLE,
                 local_A, local_rows * size, MPI_DOUBLE,
                 0, MPI_COMM_WORLD);
    
    // Each process computes its portion
    matrixVectorMult(local_A, b, local_x, local_rows, size);
    
    // Gather results back to process 0
    MPI_Gatherv(local_x, local_rows, MPI_DOUBLE,
                x_parallel, recvcounts, recvdispls, MPI_DOUBLE,
                0, MPI_COMM_WORLD);
    
    // End timing
    double end_time = MPI_Wtime();
    double parallel_time = end_time - start_time;
    
    // Process 0 outputs results
    if (rank == 0) {
        printf("Parallel_Time: %.6f\n", parallel_time);
        
        free(A);
        free(x_parallel);
        free(sendcounts);
        free(displs);
        free(recvcounts);
        free(recvdispls);
    }
    
    // Cleanup
    free(local_A);
    free(local_x);
    free(b);
    
    MPI_Finalize();
    return 0;
}
