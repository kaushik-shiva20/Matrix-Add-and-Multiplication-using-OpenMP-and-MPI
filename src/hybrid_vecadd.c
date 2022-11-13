#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include "data.h"
#include <omp.h>
#include "timer.h"

int main(int argc, char *argv[]) {

//TODO: Implement Vector addition MPI here //
    struct timespec start;
    data_struct *d_1;
    data_struct *d_2;
    data_struct *d_3;

    double *d_1_data;
    double *d_2_data;
    double *d_3_data;

    int rows_per_proc;

    int rank, size;

    if(argc != 3){
        //printf("ERROR: Please specify only 2 files.\n");
        //exit(EXIT_FAILURE);
    }
    //int numThreads = atoi(argv[3]);
    omp_set_num_threads(4);
    //printf("setting num omp threads to %d\n",numThreads);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double *currRankData_1;
    double *currRankData_2;
    double *currRankData_3;

    if(rank == 0) {

        d_1 = get_data_struct(argv[2]);
        d_2 = get_data_struct(argv[3]);

        if (d_1->cols != 1 || d_2->cols != 1) {
            printf("ERROR: The column dimension of vector is not correct\n");
            //exit(EXIT_FAILURE);
        }
        if (d_1->rows != d_2->rows) {
            printf("ERROR: The row dimension of vector is not correct\n");
            //exit(EXIT_FAILURE);
        }

        d_3 = malloc(sizeof(data_struct));
        d_3->rows = d_1->rows;
        d_3->cols = d_1->cols;

        d_1_data = malloc(d_1->rows * sizeof(double));
        d_2_data = malloc(d_2->rows * sizeof(double));
        d_3_data = malloc(d_3->rows * sizeof(double));

        for(int i=0;i<d_1->rows;i++){
            d_1_data[i] = d_1->data_point[i][0];
            d_2_data[i] = d_2->data_point[i][0];
        }

        for(int i=0;i<d_1->rows;i++){
            free(d_1->data_point[i]);
            free(d_2->data_point[i]);
        }
        free(d_1->data_point);
        free(d_2->data_point);
        rows_per_proc = d_3->rows/size;
        start_timer(&start);
    }

    MPI_Bcast(&rows_per_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);


    currRankData_1 = malloc(rows_per_proc * sizeof(double));
    currRankData_2 = malloc(rows_per_proc * sizeof(double));
    currRankData_3 = malloc(rows_per_proc * sizeof(double));

    MPI_Scatter(d_1_data, rows_per_proc, MPI_DOUBLE, currRankData_1, rows_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(d_2_data, rows_per_proc, MPI_DOUBLE, currRankData_2, rows_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < rows_per_proc; i++) {
            currRankData_3[i] = currRankData_1[i] + currRankData_2[i];
        }
    }

    MPI_Gather(currRankData_3, rows_per_proc, MPI_DOUBLE, d_3_data, rows_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if(rank == 0){
        stop_timer(&start);
        free(d_1_data);
        free(d_2_data);

        d_3->data_point = calloc(d_3->rows, sizeof(double *));
        for (int i = 0; i < d_3->rows; ++i)
            d_3->data_point[i] = calloc(d_3->cols, sizeof(double));

        for(int i=0;i<d_3->rows;i++){
            d_3->data_point[i][0] = d_3_data[i];
        }
        print_data(d_3);

        free(d_3_data);
        for (int i = 0; i < d_3->rows; ++i)
            free(d_3->data_point[i]);
        free(d_3->data_point);
    }
    free(currRankData_1);
    free(currRankData_2);
    free(currRankData_3);

    MPI_Finalize();
    return 0;
}
