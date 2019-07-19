/* file : malloc.c
 * description : implements malloc system call and other necessary functions
 * acknowledgements : 
 * Author :
 * Date Created:
 * Revisions :
 */

//todo : move all define and typedef to common.h

#define FREE_BLOCK -1
#define ALLOC_BLOCK 0

#include "../include/common.h"

typedef struct BlockMetaData{
	size_t block_size;
	struct BlockMetaData *next;
	int status;
}
