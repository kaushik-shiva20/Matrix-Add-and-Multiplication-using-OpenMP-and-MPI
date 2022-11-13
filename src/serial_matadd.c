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

    if(d_1->cols != d_2->cols || d_1->rows != d_2->rows){
        printf("ERROR: The number of columns/rows of matrix A must match the number of columns/rows of matrix B.\n");
        printf("num rows %d num cols %d", d_1->rows, d_1->cols);
        exit(EXIT_FAILURE);
    }
    
    start_timer(&start);
    /* TODO: Implement serial Matrix addition */
    data_struct *d_3 = malloc(sizeof(data_struct));
    d_3->rows = d_1->rows;
    d_3->cols = d_1->cols;
    d_3->data_point = calloc(d_3->rows, sizeof(double*));
    for(int i=0; i < d_3->rows; ++i)
        d_3->data_point[i]=calloc(d_3->cols, sizeof(double));

    for(int i = 0; i < d_1->rows; i++){
        for(int j = 0; j < d_1->cols; j++){
            d_3->data_point[i][j] = d_1->data_point[i][j] + d_2->data_point[i][j];
        }
    }
    stop_timer(&start);
    fprintf(stderr, " (calculating answer)\n");
    
    start_timer(&start);
    /* TODO: Print output */
    print_data(d_3);
    stop_timer(&start);
    fprintf(stderr, " (printing output)\n");
    
}
