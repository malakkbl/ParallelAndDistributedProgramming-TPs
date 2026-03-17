#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

/* ---------------------------------------------------------------
 * TP7 - Exercise 2 : Parallel Poisson Solver (Jacobi iteration)
 * Domain [0,1]x[0,1], exact solution u(x,y) = xy(x-1)(y-1)
 * ---------------------------------------------------------------*/

#define MAX_ITERATIONS 100000
#define TOLERANCE      1.0e-6

double exact_solution(double x, double y) {
    return x * y * (x - 1.0) * (y - 1.0);
}

double f_function(double x, double y) {
    return 2.0 * (x * x - x + y * y - y);
}

int main(int argc, char *argv[]) {
    int rank, size;
    int dims[2]    = {0, 0};
    int periods[2] = {0, 0};   /* no wrap-around: Dirichlet BC */
    int coords[2];
    MPI_Comm cart_comm;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Grid size (can be overridden on command line) */
    int Nx = 100, Ny = 100;
    if (argc > 2) { Nx = atoi(argv[1]); Ny = atoi(argv[2]); }

    /* ---- 2D Cartesian process grid ---- */
    MPI_Dims_create(size, 2, dims);
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, 2, coords);

    /* Grid spacing */
    double hx = 1.0 / (Nx - 1);
    double hy = 1.0 / (Ny - 1);

    /* Jacobi coefficients */
    double coef1 = 0.5 * (hx * hx * hy * hy) / (hx * hx + hy * hy);
    double coef2 = 1.0 / (hx * hx);
    double coef3 = 1.0 / (hy * hy);

    /* ---- Neighbors ---- */
    int neighbor_north, neighbor_east, neighbor_south, neighbor_west;
    MPI_Cart_shift(cart_comm, 1, 1, &neighbor_west,  &neighbor_east);
    MPI_Cart_shift(cart_comm, 0, 1, &neighbor_north, &neighbor_south);

    /* ---- Local subdomain size (handle remainder) ---- */
    int local_nx = Nx / dims[1] + (coords[1] < Nx % dims[1] ? 1 : 0);
    int local_ny = Ny / dims[0] + (coords[0] < Ny % dims[0] ? 1 : 0);

    /* Global starting indices */
    int start_x = coords[1] * (Nx / dims[1])
                + (coords[1] < Nx % dims[1] ? coords[1] : Nx % dims[1]);
    int start_y = coords[0] * (Ny / dims[0])
                + (coords[0] < Ny % dims[0] ? coords[0] : Ny % dims[0]);

    /* ---- Allocate local grids (with 1-cell halo) ---- */
    int rows = local_ny + 2, cols = local_nx + 2;
    double **u_old = malloc(rows * sizeof(double *));
    double **u_new = malloc(rows * sizeof(double *));
    double **f     = malloc(rows * sizeof(double *));
    for (int i = 0; i < rows; i++) {
        u_old[i] = calloc(cols, sizeof(double));
        u_new[i] = calloc(cols, sizeof(double));
        f[i]     = calloc(cols, sizeof(double));
    }

    /* ---- Initialize RHS f ---- */
    for (int i = 1; i <= local_ny; i++) {
        for (int j = 1; j <= local_nx; j++) {
            double x = (start_x + j - 1) * hx;
            double y = (start_y + i - 1) * hy;
            f[i][j]  = f_function(x, y);
        }
    }

    /* ---- Communication buffers ---- */
    double *send_n = malloc(local_nx * sizeof(double));
    double *send_s = malloc(local_nx * sizeof(double));
    double *recv_n = malloc(local_nx * sizeof(double));
    double *recv_s = malloc(local_nx * sizeof(double));
    double *send_e = malloc(local_ny * sizeof(double));
    double *send_w = malloc(local_ny * sizeof(double));
    double *recv_e = malloc(local_ny * sizeof(double));
    double *recv_w = malloc(local_ny * sizeof(double));

    if (rank == 0) {
        printf("Poisson execution with %d MPI processes\n", size);
        printf("Domain size: ntx=%d nty=%d\n", Nx, Ny);
        printf("Topology dimensions: %d along x, %d along y\n", dims[1], dims[0]);
        printf("-----------------------------------------\n");
    }
    MPI_Barrier(cart_comm);

    printf("Rank in the topology: %d   Array indices: x from %d to %d, y from %d to %d\n",
           rank, start_x + 1, start_x + local_nx, start_y + 1, start_y + local_ny);
    printf("Process %d has neighbors: N %d  E %d  S %d  W %d\n",
           rank, neighbor_north, neighbor_east, neighbor_south, neighbor_west);

    /* ================================================================
     * Jacobi iteration
     * ================================================================*/
    double global_error = 1.0;
    int    iter         = 0;
    double start_time   = MPI_Wtime();

    while (global_error > TOLERANCE && iter < MAX_ITERATIONS) {

        /* -- Pack send buffers -- */
        for (int i = 1; i <= local_ny; i++) {
            send_e[i-1] = u_old[i][local_nx];
            send_w[i-1] = u_old[i][1];
        }
        for (int j = 1; j <= local_nx; j++) {
            send_n[j-1] = u_old[1][j];
            send_s[j-1] = u_old[local_ny][j];
        }

        /* -- Halo exchange east/west -- */
        MPI_Sendrecv(send_e, local_ny, MPI_DOUBLE, neighbor_east,  0,
                     recv_w, local_ny, MPI_DOUBLE, neighbor_west,  0, cart_comm, MPI_STATUS_IGNORE);
        MPI_Sendrecv(send_w, local_ny, MPI_DOUBLE, neighbor_west,  1,
                     recv_e, local_ny, MPI_DOUBLE, neighbor_east,  1, cart_comm, MPI_STATUS_IGNORE);

        /* -- Halo exchange north/south -- */
        MPI_Sendrecv(send_n, local_nx, MPI_DOUBLE, neighbor_north, 2,
                     recv_s, local_nx, MPI_DOUBLE, neighbor_south, 2, cart_comm, MPI_STATUS_IGNORE);
        MPI_Sendrecv(send_s, local_nx, MPI_DOUBLE, neighbor_south, 3,
                     recv_n, local_nx, MPI_DOUBLE, neighbor_north, 3, cart_comm, MPI_STATUS_IGNORE);

        /* -- Unpack halos -- */
        if (neighbor_east  != MPI_PROC_NULL)
            for (int i = 1; i <= local_ny; i++) u_old[i][local_nx+1] = recv_e[i-1];
        if (neighbor_west  != MPI_PROC_NULL)
            for (int i = 1; i <= local_ny; i++) u_old[i][0]          = recv_w[i-1];
        if (neighbor_north != MPI_PROC_NULL)
            for (int j = 1; j <= local_nx; j++) u_old[0][j]          = recv_n[j-1];
        if (neighbor_south != MPI_PROC_NULL)
            for (int j = 1; j <= local_nx; j++) u_old[local_ny+1][j] = recv_s[j-1];

        /* -- Jacobi update (interior only) -- */
        double local_error = 0.0;
        for (int i = 1; i <= local_ny; i++) {
            for (int j = 1; j <= local_nx; j++) {
                /* Skip global boundary points (Dirichlet u=0) */
                if (start_x + j - 1 == 0 || start_x + j - 1 == Nx - 1 ||
                    start_y + i - 1 == 0 || start_y + i - 1 == Ny - 1)
                    continue;

                u_new[i][j] = coef1 * (coef2 * (u_old[i][j+1] + u_old[i][j-1]) +
                                       coef3 * (u_old[i+1][j]  + u_old[i-1][j]) -
                                       f[i][j]);

                double diff = fabs(u_new[i][j] - u_old[i][j]);
                if (diff > local_error) local_error = diff;
            }
        }

        /* Swap u_old <-> u_new without copying */
        double **tmp = u_old; u_old = u_new; u_new = tmp;

        MPI_Allreduce(&local_error, &global_error, 1, MPI_DOUBLE, MPI_MAX, cart_comm);
        iter++;

        if (rank == 0 && iter % 100 == 0)
            printf("Iteration %d   global_error = %e\n", iter, global_error);
    }

    double elapsed = MPI_Wtime() - start_time;
    if (rank == 0) {
        printf("Converged after %d iterations in %f seconds\n", iter, elapsed);
        printf("Exact solution u_exact - Computed solution u\n");
    }

    /* ================================================================
     * Gather results to rank 0 and validate
     * ================================================================*/
    double **global_u  = NULL;
    int    *recvcounts = NULL;   /* FIX: must be int*, not double* */
    int    *displs     = NULL;   /* FIX: must be int*, not double* */

    if (rank == 0) {
        global_u = malloc(Ny * sizeof(double *));
        for (int i = 0; i < Ny; i++)
            global_u[i] = malloc(Nx * sizeof(double));
        recvcounts = malloc(size * sizeof(int));
        displs     = malloc(size * sizeof(int));
    }

    /* Pack local interior data into flat array */
    double *local_data = malloc(local_nx * local_ny * sizeof(double));
    int idx = 0;
    for (int i = 1; i <= local_ny; i++)
        for (int j = 1; j <= local_nx; j++)
            local_data[idx++] = u_old[i][j];

    int local_size = local_nx * local_ny;
    MPI_Gather(&local_size, 1, MPI_INT, recvcounts, 1, MPI_INT, 0, cart_comm);

    if (rank == 0) {
        displs[0] = 0;
        for (int i = 1; i < size; i++)
            displs[i] = displs[i-1] + recvcounts[i-1];
    }

    double *global_data = NULL;
    if (rank == 0)
        global_data = malloc(Nx * Ny * sizeof(double));

    MPI_Gatherv(local_data, local_size, MPI_DOUBLE,
                global_data, recvcounts, displs, MPI_DOUBLE, 0, cart_comm);

    /* ---- Unpack and validate on rank 0 ---- */
    if (rank == 0) {
        int g_idx = 0;
        for (int p = 0; p < size; p++) {
            int pc[2];
            MPI_Cart_coords(cart_comm, p, 2, pc);

            int pnx = Nx / dims[1] + (pc[1] < Nx % dims[1] ? 1 : 0);
            int pny = Ny / dims[0] + (pc[0] < Ny % dims[0] ? 1 : 0);
            int psx = pc[1] * (Nx / dims[1]) + (pc[1] < Nx % dims[1] ? pc[1] : Nx % dims[1]);
            int psy = pc[0] * (Ny / dims[0]) + (pc[0] < Ny % dims[0] ? pc[0] : Ny % dims[0]);

            for (int i = 0; i < pny; i++)
                for (int j = 0; j < pnx; j++)
                    global_u[psy + i][psx + j] = global_data[g_idx++];
        }

        for (int k = 0; k < 5; k++) {
            int yi = (k + 1) * Ny / 6;
            int xi = (k + 1) * Nx / 6;
            double x = xi * hx, y = yi * hy;
            printf("%e - %e\n", exact_solution(x, y), global_u[yi][xi]);
        }

        free(global_data);
        for (int i = 0; i < Ny; i++) free(global_u[i]);
        free(global_u);
        free(recvcounts);
        free(displs);
    }

    /* ---- Cleanup ---- */
    free(local_data);
    free(send_n); free(send_s); free(recv_n); free(recv_s);
    free(send_e); free(send_w); free(recv_e); free(recv_w);
    for (int i = 0; i < rows; i++) { free(u_old[i]); free(u_new[i]); free(f[i]); }
    free(u_old); free(u_new); free(f);

    MPI_Finalize();
    return 0;
}
