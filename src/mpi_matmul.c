#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include "data.h"
#include "timer.h"

int main(int argc, char *argv[]) {

    //TODO: Implement Vector addition MPI here //
    //mpi matrix multiplication
    struct timespec start;

    data_struct *d_1;
    data_struct *d_2;
    data_struct *d_3;

    double *d_1_data;
    double *d_2_data;
    double *d_3_data;

    int rank, size;
    int from, to;

    int d1_row_size;
    int d1_col_size;
    int d2_row_size;
    int d2_col_size;

    if (argc != 3) {
        printf("ERROR: Please specify only 2 files.\n");
        //exit(EXIT_FAILURE);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double *currRankData_1;
    double *currRankData_2;
    double *currRankData_3;

    if (rank == 0) {

        d_1 = get_data_struct(argv[1]);
        d_2 = get_data_struct(argv[2]);

        if (d_1->cols != d_2->rows) {
            //printf("ERROR: The column dimension of vector is not correct\n");
            //exit(EXIT_FAILURE);
        }

        d1_row_size = d_1->rows;
        d1_col_size = d_1->cols;
        d2_row_size = d_2->rows;
        d2_col_size = d_2->cols;

        //printf("D1 - rows: %d, cols: %d | D2 - rows: %d, cols: %d\n",d_1->rows,d_1->cols,d_2->rows,d_2->cols);

        d_3 = malloc(sizeof(data_struct));
        d_3->rows = d_1->rows;
        d_3->cols = d_2->cols;

        d_1_data = malloc(d_1->rows * d_1->cols * sizeof(double));
        d_2_data = malloc(d_2->rows * d_2->cols * sizeof(double));
        d_3_data = malloc(d_3->rows * d_3->cols * sizeof(double));

        //Serialize the data 1
        for (int i = 0; i < d_1->rows; i++) {
            for (int j = 0; j < d_1->cols; j++) {
                d_1_data[i * d_1->cols + j] = d_1->data_point[i][j];
            }
        }

        //Serialize the data 2
        for (int i = 0; i < d_2->rows; i++) {
            for (int j = 0; j < d_2->cols; j++) {
                d_2_data[i * d_2->cols + j] = d_2->data_point[i][j];
            }
        }
        //printf("Data Serialization Done");
        //freeup the original containers
        for (int i = 0; i < d_1->rows; i++) {
            free(d_1->data_point[i]);
            free(d_2->data_point[i]);
        }
        free(d_1->data_point);
        free(d_2->data_point);
        start_timer(&start);
    }

    MPI_Bcast(&d1_row_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&d1_col_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&d2_row_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&d2_col_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
/*
    from = rank * d1_row_size/size;
    to = (rank+1) * d1_row_size/size;
    if(rank == size - 1){
        to = d1_row_size;
    }
*/
    if(rank != 0)
    {
        d_2_data = malloc(d2_row_size * d2_col_size * sizeof(double));
    }
    MPI_Bcast(d_2_data, d2_row_size*d2_col_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    currRankData_1 = malloc((d1_row_size/size) * d1_col_size * sizeof(double));
    //currRankData_2 = malloc(d2_matrix_size * sizeof(double));
    currRankData_3 = calloc((d1_row_size/size) * d2_col_size , sizeof(double));

    MPI_Scatter (d_1_data, (d1_row_size/size)*d1_col_size, MPI_DOUBLE, currRankData_1, (d1_row_size/size)*d1_col_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    //MPI_Scatter(d_1_data, d2_matrix_size/size, MPI_DOUBLE, currRankData_1, d2_matrix_size/size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    //MPI_Scatter(d_2_data, d2_matrix_size, MPI_DOUBLE, currRankData_2, d2_matrix_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    from = rank * d1_row_size/size;
    if(rank == size-1)
    {
        to = d1_row_size - (size-1)*(d1_row_size/size);
    }else
    {
        to = d1_row_size/size;
    }

    for (int i=0; i<to; i++) {
        for (int j = 0; j < d2_col_size; j++) {
            for (int k = 0; k < d1_col_size; k++) {
                currRankData_3[i * d2_col_size + j] += currRankData_1[i * d1_col_size + k] * d_2_data[k * d2_col_size + j];
            }
        }
    }
/*
    if(rank == size - 1) {
        MPI_Gather(currRankData_3, ((d1_row_size * d2_col_size) - ((size-1) * ((d1_row_size / size) * d2_col_size))), MPI_DOUBLE, &d_3_data[from], ((d1_row_size * d2_col_size) - ((size-1) * ((d1_row_size / size) * d2_col_size))), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }else{
        MPI_Gather(currRankData_3, (d1_row_size / size) * d2_col_size, MPI_DOUBLE, &d_3_data[from], (d1_row_size / size) * d2_col_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
*/
    MPI_Gather(currRankData_3, (d1_row_size / size) * d2_col_size, MPI_DOUBLE, d_3_data, (d1_row_size / size) * d2_col_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //MPI_Gather(currRankData_3, rows_per_proc * d2_col_size, MPI_DOUBLE, d_3_data, rows_per_proc * d2_col_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        stop_timer(&start);
        free(d_1_data);
        free(d_2_data);
        d_3->data_point = calloc(d_3->rows, sizeof(double *));
        for (int i = 0; i < d_3->rows; ++i)
            d_3->data_point[i] = calloc(d_3->cols, sizeof(double));
        for (int i = 0; i < d_3->rows; i++) {
            for (int j = 0; j < d_3->cols; j++) {
                d_3->data_point[i][j] = d_3_data[i * d_3->cols + j];
            }
        }
        print_data(d_3);
        free(d_3_data);
        for (int i = 0; i < d_3->rows; ++i)
            free(d_3->data_point[i]);
        free(d_3->data_point);
    }
    free(currRankData_1);
    //free(currRankData_2);
    free(currRankData_3);

    MPI_Finalize();
    return 0;

}
