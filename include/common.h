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
#include <sys/mman.h>


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


typedef struct zoneinfo{
	size_t block_size;
	struct zoneinfo *next;
	int status;
}ZoneInfo;


/* Macros */
#define BLOCK_META_SIZE sizeof(BlockMeta)
#define DEFAULT_ALLOC_SIZE 1 * sysconf(_SC_PAGESIZE)
#define UNUSABLE_MEMORY_SIZE 8 + BLOCK_META_SIZE
#define FREE_BLOCK 1
#define ALLOCATED_BLOCK 0
#define ALLOCATED_ZONE 0
#define FREE ZONE 1
#define BLOCK_MEM(ptr) ((void *)((unsigned long)ptr + sizeof(BlockMeta)))
#define BLOCK_HEADER(ptr) ((void *)((unsigned long)ptr - sizeof(BlockMeta)))
#define DEFAULT_MMAP_THRESHOLD_MIN (128 * 1024)
#define DEFAULT_MMAP_THRESHOLD DEFAULT_MMAP_THRESHOLD_MIN
#ifndef DEFAULT_MMAP_THRESHOLD_MAX
  /* For 32-bit platforms we cannot increase the maximum mmap
     threshold much because it is also the minimum value for the
     maximum heap size and its alignment.  Going above 512k (i.e., 1M
     for new heaps) wastes too much address space.  */
# if __WORDSIZE == 32
#  define DEFAULT_MMAP_THRESHOLD_MAX (512 * 1024)
# else
#  define DEFAULT_MMAP_THRESHOLD_MAX (4 * 1024 * 1024 * sizeof(long))
# endif
#endif


#define MMAP(addr, size, prot, flags) \
 mmap((addr), (size), (prot), (flags)|MAP_ANONYMOUS|MAP_PRIVATE, -1, 0)

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



#endif
