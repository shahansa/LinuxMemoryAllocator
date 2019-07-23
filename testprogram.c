#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "include/common.h"
#include "include/commonFunctions.h"

/* returns an array of arrays of char*, all of which NULL */
char ***alloc_matrix(unsigned rows, unsigned columns) {
    char ***matrix = u_malloc(rows * sizeof(char **));
    unsigned row = 0;
    unsigned column = 0;
    if (!matrix) abort();

    for (row = 0; row < rows; row++) {
        matrix[row] = u_calloc(columns, sizeof(char *));
        if (!matrix[row]) abort();
        for (column = 0; column < columns; column++) {
            matrix[row][column] = NULL;
        }
    }
    return matrix;
}

/* deallocates an array of arrays of char*, calling free() on each */
void free_matrix(char ***matrix, unsigned rows, unsigned columns) {
    unsigned row = 0;
    unsigned column = 0;
    for (row = 0; row < rows; row++) {
        for (column = 0; column < columns; column++) {
            /*    printf("column %d row %d\n", column, row);*/
            u_free(matrix[row][column]);
        }
        u_free(matrix[row]);
    }
    u_free(matrix);
}


int main(int agrc, char **argv) {
     int i;
    srand(time(NULL));
    int randomnumber;
    int size = 1024;
    void *p[size];
    for (i = 0; i < size; i++) {
        randomnumber = rand() % 10;
        p[i] = u_malloc(1024 * 1024 * randomnumber);
    }

    for (i = size-1; i >= 0; i--) {
        u_free(p[i]);
    }

    int x = 1024;
    char *** matrix = alloc_matrix(x, x);
    free_matrix(matrix, x, x);
    return (0);
}
