#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    int value;
    int next_rank, prev_rank;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Calculate next and previous ranks in the ring
    next_rank = (rank + 1) % size;
    prev_rank = (rank - 1 + size) % size;
    
    // Process 0 reads the initial value
    if (rank == 0) {
        printf("Process 0: Enter a value: ");
        fflush(stdout);
        scanf("%d", &value);
        printf("Process 0: Read value = %d\n", value);
        fflush(stdout);
    }
    
    // Ring communication
    if (rank == 0) {
        // Process 0 sends to process 1
        MPI_Send(&value, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
        printf("Process 0: Sent %d to Process %d\n", value, next_rank);
        fflush(stdout);
    } else {
        // Receive from previous process
        MPI_Recv(&value, 1, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, &status);
        printf("Process %d: Received %d from Process %d\n", rank, value, prev_rank);
        fflush(stdout);
        
        // Add rank to the value
        value += rank;
        printf("Process %d: Added rank, new value = %d\n", rank, value);
        fflush(stdout);
        
        // Send to next process (if not the last one)
        if (rank < size - 1) {
            MPI_Send(&value, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
            printf("Process %d: Sent %d to Process %d\n", rank, value, next_rank);
            fflush(stdout);
        } else {
            printf("Process %d: Final value = %d\n", rank, value);
            fflush(stdout);
        }
    }
    
    MPI_Finalize();
    return 0;
}
