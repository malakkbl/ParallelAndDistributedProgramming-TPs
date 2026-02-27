#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>

#define N_FEATURES 5
#define MAX_EPOCHS 1000
#define LEARNING_RATE 0.01
#define THRESHOLD 1e-2

typedef struct {
    double x[N_FEATURES];
    double y;
} Sample;

Sample* generate_data(int n_samples) {
    Sample *data = (Sample*)malloc(n_samples * sizeof(Sample));
    double true_weights[N_FEATURES] = {2.5, -1.3, 0.8, 3.2, -0.5};
    
    srand(42);
    for (int i = 0; i < n_samples; i++) {
        for (int j = 0; j < N_FEATURES; j++) {
            data[i].x[j] = ((double)rand() / RAND_MAX) * 10.0 - 5.0;
        }
        data[i].y = 0.0;
        for (int j = 0; j < N_FEATURES; j++) {
            data[i].y += true_weights[j] * data[i].x[j];
        }
        data[i].y += ((double)rand() / RAND_MAX) * 0.5 - 0.25;
    }
    return data;
}

MPI_Datatype create_sample_type() {
    MPI_Datatype sample_type;
    int blocklengths[2] = {N_FEATURES, 1};
    MPI_Aint displacements[2];
    MPI_Datatype types[2] = {MPI_DOUBLE, MPI_DOUBLE};
    
    Sample dummy_sample;
    (void)dummy_sample; //to avoid a warning that ive got
    MPI_Aint base_address;
    MPI_Get_address(&dummy_sample, &base_address);
    MPI_Get_address(&dummy_sample.x, &displacements[0]);
    MPI_Get_address(&dummy_sample.y, &displacements[1]);
    
    displacements[0] = MPI_Aint_diff(displacements[0], base_address);
    displacements[1] = MPI_Aint_diff(displacements[1], base_address);
    
    MPI_Type_create_struct(2, blocklengths, displacements, types, &sample_type);
    MPI_Type_commit(&sample_type);
    
    return sample_type;
}

int main(int argc, char** argv) {
    int rank, num_procs;
    double start_time, end_time, parallel_time;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    
    // Parse command-line argument for number of samples
    int n_samples = 1000;  // Default
    if (argc > 1) {
        n_samples = atoi(argv[1]);
        if (n_samples <= 0) {
            if (rank == 0) {
                fprintf(stderr, "Invalid number of samples: %s\n", argv[1]);
                fprintf(stderr, "Usage: %s <n_samples>\n", argv[0]);
            }
            MPI_Finalize();
            return 1;
        }
    }
    
    MPI_Datatype sample_type = create_sample_type();
    
    // Generate dataset
    Sample *dataset = NULL;
    if (rank == 0) {
        dataset = generate_data(n_samples);
    }
    
    // Work distribution
    int samples_per_proc = n_samples / num_procs;
    int extra_samples = n_samples % num_procs;
    int local_n_samples = samples_per_proc + (rank < extra_samples ? 1 : 0);
    
    int *sendcounts = NULL, *displs = NULL;
    if (rank == 0) {
        sendcounts = (int*)malloc(num_procs * sizeof(int));
        displs = (int*)malloc(num_procs * sizeof(int));
        int offset = 0;
        for (int i = 0; i < num_procs; i++) {
            sendcounts[i] = samples_per_proc + (i < extra_samples ? 1 : 0);
            displs[i] = offset;
            offset += sendcounts[i];
        }
    }
    
    Sample *local_dataset = (Sample*)malloc(local_n_samples * sizeof(Sample));
    
    // Scatter dataset to all processes
    MPI_Scatterv(dataset, sendcounts, displs, sample_type,
                 local_dataset, local_n_samples, sample_type,
                 0, MPI_COMM_WORLD);
    
    // Initialize weights
    double weights[N_FEATURES];
    if (rank == 0) {
        srand(42);  // Fixed seed for reproducibility
        for (int i = 0; i < N_FEATURES; i++) {
            weights[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        }
    }
    
    // Synchronize before timing
    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();
    
    // Training loop
    int converged = 0;
    int final_epoch = 0;
    double final_loss = 0.0;
    
    for (int epoch = 0; epoch < MAX_EPOCHS && !converged; epoch++) {
        final_epoch = epoch;
        
        // Broadcast current weights to all processes
        MPI_Bcast(weights, N_FEATURES, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        // Compute local gradients and loss
        double local_gradients[N_FEATURES] = {0.0};
        double local_loss = 0.0;
        
        for (int i = 0; i < local_n_samples; i++) {
            // Prediction
            double prediction = 0.0;
            for (int j = 0; j < N_FEATURES; j++) {
                prediction += weights[j] * local_dataset[i].x[j];
            }
            
            // Error
            double error = prediction - local_dataset[i].y;
            local_loss += error * error;
            
            // Gradients
            for (int j = 0; j < N_FEATURES; j++) {
                local_gradients[j] += error * local_dataset[i].x[j];
            }
        }
        
        // Reduce gradients and loss
        double global_gradients[N_FEATURES];
        double global_loss;
        
        MPI_Reduce(local_gradients, global_gradients, N_FEATURES,
                   MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&local_loss, &global_loss, 1,
                   MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        
        // Update weights on process 0
        if (rank == 0) {
            global_loss = global_loss / n_samples;
            final_loss = global_loss;
            
            for (int j = 0; j < N_FEATURES; j++) {
                weights[j] -= LEARNING_RATE * (global_gradients[j] / n_samples);
            }
            
            // Print progress every 10 epochs (like in the example)
            if (epoch % 10 == 0) {
                printf("Epoch %d | Loss (MSE): %.6f | w[0]: %.4f, w[1]: %.4f\n",
                       epoch, global_loss, weights[0], weights[1]);
            }
            
            // Check convergence
            if (global_loss < THRESHOLD) {
                printf("Early stopping at epoch %d - loss %.5e < %.0e\n",
                       epoch, global_loss, THRESHOLD);
                converged = 1;
            }
        }
        
        // Broadcast convergence flag
        MPI_Bcast(&converged, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    
    // Synchronize after training
    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    parallel_time = end_time - start_time;
    
    // Print results
    if (rank == 0) {
        printf("\nTraining time: %.3f seconds (MPI)\n", parallel_time);
        
        // For plotting
        printf("\n CSV OUTPUT \n");
        printf("N_SAMPLES,N_PROCESSES,PARALLEL_TIME,EPOCHS,FINAL_LOSS\n");
        printf("%d,%d,%.6f,%d,%.6f\n",
               n_samples, num_procs, parallel_time, final_epoch + 1, final_loss);
    }
    
    // Cleanup
    MPI_Type_free(&sample_type);
    free(local_dataset);
    if (rank == 0) {
        free(dataset);
        free(sendcounts);
        free(displs);
    }
    
    MPI_Finalize();
    return 0;
}
