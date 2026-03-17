#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

/* ---------------------------------------------------------------
 * TP7 - Exercise 1 : Parallel Game of Life
 * 2D Cartesian MPI topology, periodic boundary conditions
 * ---------------------------------------------------------------*/

#define DEFAULT_NX      20
#define DEFAULT_NY      20
#define DEFAULT_GENERATIONS 20
#define INIT_PROB       0.3   /* probability a cell starts alive */

#define DEAD  0
#define ALIVE 1

/* Print the local subgrid owned by this rank */
void print_grid(int rank, int nx, int ny, int *grid, int generation) {
    printf("Rank %d - Generation %d:\n", rank, generation);
    for (int i = 1; i <= ny; i++) {
        for (int j = 1; j <= nx; j++) {
            printf("%d ", grid[i * (nx + 2) + j]);
        }
        printf("\n");
    }
    printf("\n");
}

/* Count the 8 live neighbors of cell (i,j) — halos must be filled first */
int count_neighbors(int nx, int ny, int *grid, int i, int j) {
    int count = 0;
    for (int di = -1; di <= 1; di++)
        for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue;
            count += grid[(i + di) * (nx + 2) + (j + dj)];
        }
    return count;
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int global_nx    = DEFAULT_NX;
    int global_ny    = DEFAULT_NY;
    int generations  = DEFAULT_GENERATIONS;
    int print_interval = 10;

    if (argc > 1) global_nx      = atoi(argv[1]);
    if (argc > 2) global_ny      = atoi(argv[2]);
    if (argc > 3) generations    = atoi(argv[3]);
    if (argc > 4) print_interval = atoi(argv[4]);

    /* ---- 2D Cartesian communicator (periodic in both directions) ---- */
    int dims[2]    = {0, 0};
    int periods[2] = {1, 1};   /* wrap-around in x and y */
    int coords[2];
    MPI_Comm cart_comm;

    MPI_Dims_create(size, 2, dims);
    if (rank == 0) {
        printf("Process grid: %d x %d\n", dims[0], dims[1]);
        printf("Global grid : %d x %d\n", global_nx, global_ny);
        printf("Generations : %d\n\n", generations);
    }

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, 2, coords);

    /* ---- Local subdomain dimensions (handle remainder) ---- */
    int local_nx = global_nx / dims[1] + (coords[1] < global_nx % dims[1] ? 1 : 0);
    int local_ny = global_ny / dims[0] + (coords[0] < global_ny % dims[0] ? 1 : 0);

    /* Global starting indices */
    int start_x = coords[1] * (global_nx / dims[1])
                + (coords[1] < global_nx % dims[1] ? coords[1] : global_nx % dims[1]);
    int start_y = coords[0] * (global_ny / dims[0])
                + (coords[0] < global_ny % dims[0] ? coords[0] : global_ny % dims[0]);
    (void)start_x; (void)start_y;   /* used for debugging if needed */

    /* ---- Allocate local grids with 1-cell halo border ---- */
    int *grid     = (int *)calloc((local_nx + 2) * (local_ny + 2), sizeof(int));
    int *new_grid = (int *)calloc((local_nx + 2) * (local_ny + 2), sizeof(int));
    if (!grid || !new_grid) {
        fprintf(stderr, "Memory allocation failed on process %d\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* ---- Find cardinal neighbors ---- */
    int north, south, east, west;
    MPI_Cart_shift(cart_comm, 0, 1, &north, &south);
    MPI_Cart_shift(cart_comm, 1, 1, &west,  &east);

    /* ---- Find diagonal neighbors (needed for 8-neighbor Game of Life) ---- */
    int north_coords[2], south_coords[2], east_coords[2], west_coords[2];
    MPI_Cart_coords(cart_comm, north, 2, north_coords);
    MPI_Cart_coords(cart_comm, south, 2, south_coords);
    MPI_Cart_coords(cart_comm, east,  2, east_coords);
    MPI_Cart_coords(cart_comm, west,  2, west_coords);

    int ne, se, sw, nw;
    int ne_c[2] = {north_coords[0], east_coords[1]};
    int se_c[2] = {south_coords[0], east_coords[1]};
    int sw_c[2] = {south_coords[0], west_coords[1]};
    int nw_c[2] = {north_coords[0], west_coords[1]};
    MPI_Cart_rank(cart_comm, ne_c, &ne);
    MPI_Cart_rank(cart_comm, se_c, &se);
    MPI_Cart_rank(cart_comm, sw_c, &sw);
    MPI_Cart_rank(cart_comm, nw_c, &nw);

    printf("Process %d at (%d,%d) | neighbors: N=%d S=%d E=%d W=%d "
           "NE=%d SE=%d SW=%d NW=%d\n",
           rank, coords[0], coords[1],
           north, south, east, west, ne, se, sw, nw);

    /* ---- Communication buffers ---- */
    int *send_n = malloc(local_nx * sizeof(int));
    int *send_s = malloc(local_nx * sizeof(int));
    int *recv_n = malloc(local_nx * sizeof(int));
    int *recv_s = malloc(local_nx * sizeof(int));
    int *send_e = malloc(local_ny * sizeof(int));
    int *send_w = malloc(local_ny * sizeof(int));
    int *recv_e = malloc(local_ny * sizeof(int));
    int *recv_w = malloc(local_ny * sizeof(int));
    int send_ne, send_se, send_sw, send_nw;
    int recv_ne, recv_se, recv_sw, recv_nw;

    /* ---- Random initialization (different seed per process) ---- */
    srand((unsigned)(time(NULL) + rank * 1000));
    for (int i = 1; i <= local_ny; i++)
        for (int j = 1; j <= local_nx; j++)
            grid[i * (local_nx + 2) + j] =
                (rand() / (double)RAND_MAX < INIT_PROB) ? ALIVE : DEAD;

    if (rank == 0) printf("\nStarting Game of Life simulation...\n\n");

    /* Always print generation 0 */
    print_grid(rank, local_nx, local_ny, grid, 0);

    /* ================================================================
     * Main simulation loop
     * ================================================================*/
    for (int gen = 1; gen <= generations; gen++) {

        /* -- Pack edges into send buffers -- */
        for (int j = 1; j <= local_nx; j++) {
            send_n[j-1] = grid[1          * (local_nx + 2) + j];
            send_s[j-1] = grid[local_ny   * (local_nx + 2) + j];
        }
        for (int i = 1; i <= local_ny; i++) {
            send_e[i-1] = grid[i * (local_nx + 2) + local_nx];
            send_w[i-1] = grid[i * (local_nx + 2) + 1];
        }
        /* Corners */
        send_ne = grid[1        * (local_nx + 2) + local_nx];
        send_se = grid[local_ny * (local_nx + 2) + local_nx];
        send_sw = grid[local_ny * (local_nx + 2) + 1];
        send_nw = grid[1        * (local_nx + 2) + 1];

        /* -- Exchange rows (north/south) -- */
        MPI_Sendrecv(send_n, local_nx, MPI_INT, north, 0,
                     recv_s, local_nx, MPI_INT, south, 0, cart_comm, MPI_STATUS_IGNORE);
        MPI_Sendrecv(send_s, local_nx, MPI_INT, south, 1,
                     recv_n, local_nx, MPI_INT, north, 1, cart_comm, MPI_STATUS_IGNORE);

        /* -- Exchange columns (east/west) -- */
        MPI_Sendrecv(send_e, local_ny, MPI_INT, east, 2,
                     recv_w, local_ny, MPI_INT, west, 2, cart_comm, MPI_STATUS_IGNORE);
        MPI_Sendrecv(send_w, local_ny, MPI_INT, west, 3,
                     recv_e, local_ny, MPI_INT, east, 3, cart_comm, MPI_STATUS_IGNORE);

        /* -- Exchange corners -- */
        MPI_Sendrecv(&send_ne, 1, MPI_INT, ne, 4, &recv_sw, 1, MPI_INT, sw, 4, cart_comm, MPI_STATUS_IGNORE);
        MPI_Sendrecv(&send_se, 1, MPI_INT, se, 5, &recv_nw, 1, MPI_INT, nw, 5, cart_comm, MPI_STATUS_IGNORE);
        MPI_Sendrecv(&send_sw, 1, MPI_INT, sw, 6, &recv_ne, 1, MPI_INT, ne, 6, cart_comm, MPI_STATUS_IGNORE);
        MPI_Sendrecv(&send_nw, 1, MPI_INT, nw, 7, &recv_se, 1, MPI_INT, se, 7, cart_comm, MPI_STATUS_IGNORE);

        /* -- Unpack halos -- */
        for (int j = 1; j <= local_nx; j++) {
            grid[0             * (local_nx + 2) + j] = recv_n[j-1];
            grid[(local_ny+1)  * (local_nx + 2) + j] = recv_s[j-1];
        }
        for (int i = 1; i <= local_ny; i++) {
            grid[i * (local_nx + 2) + 0]           = recv_w[i-1];
            grid[i * (local_nx + 2) + (local_nx+1)] = recv_e[i-1];
        }
        grid[0            * (local_nx + 2) + 0]            = recv_nw;
        grid[0            * (local_nx + 2) + (local_nx+1)] = recv_ne;
        grid[(local_ny+1) * (local_nx + 2) + 0]            = recv_sw;
        grid[(local_ny+1) * (local_nx + 2) + (local_nx+1)] = recv_se;

        /* -- Apply Conway's rules -- */
        for (int i = 1; i <= local_ny; i++) {
            for (int j = 1; j <= local_nx; j++) {
                int idx       = i * (local_nx + 2) + j;
                int neighbors = count_neighbors(local_nx, local_ny, grid, i, j);

                if (grid[idx] == ALIVE)
                    new_grid[idx] = (neighbors == 2 || neighbors == 3) ? ALIVE : DEAD;
                else
                    new_grid[idx] = (neighbors == 3) ? ALIVE : DEAD;
            }
        }

        /* Swap pointers */
        int *tmp = grid; grid = new_grid; new_grid = tmp;

        if (gen % print_interval == 0)
            print_grid(rank, local_nx, local_ny, grid, gen);
    }

    /* ---- Cleanup ---- */
    free(grid); free(new_grid);
    free(send_n); free(send_s); free(recv_n); free(recv_s);
    free(send_e); free(send_w); free(recv_e); free(recv_w);

    MPI_Finalize();
    return 0;
}
