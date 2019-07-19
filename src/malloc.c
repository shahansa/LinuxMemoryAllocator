/* file : malloc.c
 * description : implements malloc system call and other necessary functions
 * acknowledgements : 
 * Author :
 * Date Created:
 * Revisions :
 */

//todo : move all define and typedef to common.h

#define FREE_BLOCK -1
#define ALLOCATED_BLOCK 0

#include "../include/common.h"
#include <assert.h>

/*
+----------------+-------------------------------------++
|free?  |pointer |                                     |
+----------------+                                     |
| Block Meta data|  Actual memory block                |
|                |  (Actual memory requested by the    |
|(This is extra  |  program)                           |
|space used by   |                                     |
|every block)    |                                     |
|                |                                     |
|                |                                     |
+----------------+-----------+-------------------------+
 Total size = Requested Size + Block meta data size
*/

typedef struct BlockMetaData{
	size_t block_size;
	struct BlockMetaData *next;
	int status;
}BlockMeta;

#define BLOCK_META_SIZE sizeof(BlockMeta)

void *head = NULL;

BlockMeta requestToOperatingSystem(size_t size)
{
	/*refer to man page for sbrk */
	size_t requestSize = size + BLOCK_META_SIZE;
	presentBreak = sbrk(0);
	allocatedBlock = sbrk(requestSize);
	if((void*)-1 == allocatedBlock)
		return NULL;
	presentBreak -> block_size = requestSize;
	presentBreak -> status = ALLOCATED_BLOCK;
	presentBreak -> next = NULL;
	return(presentBreak);
}

void* malloc(size_t size)
{
	assert(size > 0);
	if(NULL == head){ /* this is the first request since program start*/
		head = requestToOperatingSystem(size);
		return(head);
	}
	else{
		//allocate and do other stuff tomorrow 
	}
}


	
