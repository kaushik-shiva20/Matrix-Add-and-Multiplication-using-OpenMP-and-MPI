#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "data.h"
#include "timer.h"

int main(int argc, char **argv)
{
    if(argc != 3){
        printf("ERROR: Please specify only 2 files.\n");
        exit(EXIT_FAILURE);
    }

    struct timespec start;
    start_timer(&start);

    data_struct *d_1 = get_data_struct(argv[1]);
    data_struct *d_2 = get_data_struct(argv[2]);
    
    stop_timer(&start);
    fprintf(stderr, " (reading input)\n");

    //Vector is of size (nx1), it will always return coloumn =1
    if(d_1->cols != 1 || d_2->cols !=1){
        printf("ERROR: The column dimension of vector is not correct\n");
        exit(EXIT_FAILURE);
    }

    if(d_1->rows != d_2->rows){
        printf("ERROR: The row dimension of vector is not correct\n");
        exit(EXIT_FAILURE);
    }

    start_timer(&start);
    /* TODO: Implement serial Vector Addition */
    data_struct *d_3 = malloc(sizeof(data_struct));
    d_3->rows = d_1->rows;
    d_3->cols = d_1->cols;
    d_3->data_point = calloc(d_3->rows, sizeof(double*));
    for(int i=0; i < d_3->rows; ++i)
        d_3->data_point[i]=calloc(d_3->cols, sizeof(double));

    for(int i = 0; i < d_1->rows; i++){
        d_3->data_point[i][0] = d_1->data_point[i][0] + d_2->data_point[i][0];
    }
    stop_timer(&start);
    fprintf(stderr, " (calculating answer)\n");
    
    start_timer(&start);
    /* TODO: Print output */
    print_data(d_3);
    stop_timer(&start);
    fprintf(stderr, " (printing output)\n");
    
}
