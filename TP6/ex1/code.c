#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROWS 4
#define COLS 5

void print_matrix(int matrix[][COLS], int rows, int cols, int rank, const char* title) {
    printf("\nProcess %d - %s:\n", rank, title);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%3d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    fflush(stdout);
}

void print_transposed_matrix(int matrix[][ROWS], int rows, int cols, int rank, const char* title) {
    printf("\nProcess %d - %s:\n", rank, title);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%3d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    fflush(stdout);
}

int main(int argc, char** argv) {
    int rank, size;
    int matrix[ROWS][COLS];
    int transposed[COLS][ROWS];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Check if we have exactly 2 processes
    if (size != 2) {
        if (rank == 0) {
            fprintf(stderr, "This program requires exactly 2 processes!\n");
        }
        MPI_Finalize();
        return 1;
    }
    
    // Step 1: Initialize matrix on process 0
    if (rank == 0) {
        int value =1;
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                matrix[i][j] = value++;
            }
        }
        print_matrix(matrix, ROWS, COLS, rank, "Original Matrix a");
    }
    
    // Step 2: Create derived datatype for column extraction
    MPI_Datatype column_type;
    MPI_Type_vector(
        ROWS,           // count: number of blocks (elements in a column)
        1,              // blocklength: elements per block
        COLS,           // stride: distance between blocks (jump to next row)
        MPI_INT,        // oldtype: base datatype
        &column_type    // newtype: output derived type
    );
    MPI_Type_commit(&column_type);
    
    // Step 3: Resize the type for proper extent
    MPI_Datatype column_type_resized;
    MPI_Type_create_resized(
        column_type,            // input type
        0,                      // lower bound
        sizeof(int),            // extent: 1 integer (distance to next column start)
        &column_type_resized    // output type
    );
    MPI_Type_commit(&column_type_resized);
    
    // Step 4: Send from process 0
    if (rank == 0) {
        MPI_Send(
            &matrix[0][0],          // buffer: start of matrix
            COLS,                   // count: number of columns to send
            column_type_resized,    // datatype: our custom column type
            1,                      // dest: process 1
            0,                      // tag
            MPI_COMM_WORLD
        );
    }
    
    // Step 5: Receive on process 1
    if (rank == 1) {
        MPI_Recv(
            &transposed[0][0],  // buffer: start of transposed matrix
            ROWS * COLS,        // count: total number of elements
            MPI_INT,            // datatype: regular integers
            0,                  // source: process 0
            0,                  // tag
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
        print_transposed_matrix(transposed, COLS, ROWS, rank, "Matrix transposed at");
    }
    
    // Step 6: Cleanup derived types
    MPI_Type_free(&column_type);
    MPI_Type_free(&column_type_resized);
    
    MPI_Finalize();
    return 0;
}
