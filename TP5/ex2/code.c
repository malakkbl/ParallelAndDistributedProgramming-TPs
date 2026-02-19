#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    int value;
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // I chose to read the input from p0
    if (rank == 0) {
        fflush(stdout);
        scanf("%d", &value);
    }
    
    // This is to distribute the values to all other processes
    MPI_Bcast(&value, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    
    printf("Process %d got %d\n", rank, value);
    fflush(stdout);
    
    // This is to keep reading until negative value
    while (value >= 0) {
        if (rank == 0) {
            fflush(stdout);
            scanf("%d", &value);
        }
        
        MPI_Bcast(&value, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        if (value >= 0) {
            printf("Process %d got %d\n", rank, value);
            fflush(stdout);
        }
    }
    
    MPI_Finalize();
    return 0;
}
