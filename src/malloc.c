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
#define DEBUG_LEVEL_1

#include "../include/common.h"
#include "../include/commonFunctions.h"
#include <assert.h>

/*
+----------------+-------------------------------------+
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

BlockMeta* findCompatibleBlock(BlockMeta *head, size_t request_size)
{
#ifdef DEBUG_LEVEL_1
	printf("\nin function %s ,line %d",__func__, __LINE__ );
	printf("\nRequest to find block for %ld Bytes",request_size);
#endif
	assert(head != NULL);
	assert(request_size > 0);
	BlockMeta *temp = head;
	while(NULL != temp){
		if(temp->block_size >= request_size && temp->status != ALLOCATED_BLOCK){
#ifdef DEBUG_LEVEL_1
			printf("\nReturn from function %s value %ld",__func__, temp);
#endif
			return(temp);
		}
		temp = temp->next;
	}
#ifdef DEBUG_LEVEL_1
	printf("\nReturn from function %s value NULL",__func__);
#endif
	return(NULL);
	
}

void*  requestToOperatingSystem(size_t size)
{
	/*refer to man page for sbrk */
	//TODO : see if size is greater than threshold to use mmap
#ifdef DEBUG_LEVEL_1
	printf("\nin function %s",__func__);
	printf("\nrequest to allocate %ld Bytes",size);
#endif
	size_t requestSize = size + BLOCK_META_SIZE;
	BlockMeta *presentBreak = sbrk(0);
	void *allocatedBlock = sbrk((intptr_t)requestSize);
	assert((void *)presentBreak == allocatedBlock);
	if((void*)-1 == allocatedBlock){
#ifdef DEBUG_LEVEL_1
	printf("\nReturn from function %s value NULL",__func__);
#endif
		return NULL;
	}
	presentBreak->block_size = requestSize;
	presentBreak->status = ALLOCATED_BLOCK;
	presentBreak->next = NULL;
#ifdef DEBUG_LEVEL_1
	printf("\nReturn from function %s value %ld", __func__, presentBreak);
#endif
	return(presentBreak);
}

void* u_malloc(size_t size)
{
	assert(size > 0);
	BlockMeta *block = NULL;
	if(NULL == head){ /* this is the first request since program start*/
		head = requestToOperatingSystem(size);
		block = head;
	}
	else{
		BlockMeta *compatibleBlock = NULL;
		compatibleBlock = findCompatibleBlock(head, size);
		if(NULL != compatibleBlock){
			block = compatibleBlock;
			block->status = ALLOCATED_BLOCK;
		}
		else{
			BlockMeta *last_block = head;
			while(NULL != last_block->next)
				last_block = last_block->next;
			block = requestToOperatingSystem(size);
			last_block->next = block;
			block->status = ALLOCATED_BLOCK;
		}
	}
	return(block+1);
}

void u_free(void *memptr)
{
	BlockMeta *ptr = (BlockMeta*)memptr;
	BlockMeta *block_ptr = ptr-1;
	block_ptr->status = FREE_BLOCK;
}
	
