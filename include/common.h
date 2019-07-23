#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

#define ERROR -1
#define INVALID ERROR
#define UNDEF ERROR

#define BUF_LEN 256
#define INFTY 1000000
#define EPS 0.0001

#define SUCCESS 0
#define FAILURE 1

typedef struct BlockMetaData{
	size_t block_size;
	struct BlockMetaData *next;
	int status;
}BlockMeta;


/* Macros */
#define BLOCK_META_SIZE sizeof(BlockMeta)
#define DEFAULT_ALLOC_SIZE 1 * sysconf(_SC_PAGESIZE)
#define UNUSABLE_MEMORY_SIZE 8 + BLOCK_META_SIZE
#define FREE_BLOCK 1
#define ALLOCATED_BLOCK 0
#define DEBUG_LEVEL_1
#define BLOCK_MEM(ptr) ((void *)((unsigned long)ptr + sizeof(BlockMeta)))
#define BLOCK_HEADER(ptr) ((void *)((unsigned long)ptr - sizeof(BlockMeta)))

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef SQR
#define SQR(x) ((x) * (x))
#endif

#define ERR_MESG(x) { perror(x); return -1; }
#if DEBUG
//#ifdef __GNUC__
#define dprintf(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
#else
#define dprintf(format, ...)
#endif

#define Malloc(n,type) (type *) malloc( (unsigned) ((n)*sizeof(type)))
#define Realloc(loc,n,type) (type *) realloc( (char *)(loc), \
                                              (unsigned) ((n)*sizeof(type)))

#define matrix_alloc(mat,rows,cols,type) {                        \
    int ii;                                                       \
    type *temp;                                                   \
                                                                  \
    if (NULL == (temp = (type *) malloc(rows*cols*sizeof(type))) || \
        NULL == (mat = (type **) malloc(rows * sizeof(type *))))  \
        ERR_MESG("Out of memory");                                \
    for (ii = 0; ii < rows; temp += cols, ii++)                   \
        mat[ii] = temp;                                           \
  }

#define matrix_free(mat) {                      \
    free(mat[0]);                               \
    free(mat);                                  \
  }

#endif
