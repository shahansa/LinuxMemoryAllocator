/* file : malloc.c
 * description : implements malloc system call and other necessary functions
 * acknowledgements :
 * Author :
 * Date Created:
 * Revisions :
 */

//todo : move all define and typedef to common.h
#include "../include/common.h"
#include "../include/commonFunctions.h"
#include <assert.h>
#include <math.h>
#include <unistd.h>
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


/* Global variable */
static BlockMeta *head = NULL;
BlockMeta *last_allocated = NULL; /*used in last fit scheme*/


// Algorithm: cannot change the data
BlockMeta* findCompatibleBlock_first_fit(BlockMeta *head, size_t request_size)
{
	#ifdef DEBUG_LEVEL_1
		printf("In Function: %s ,line %d\n",__func__, __LINE__ );
		printf("Request to find block for %ld Bytes\n",request_size);
	#endif
	assert(head != NULL);
	assert(request_size > 0);
	BlockMeta *temp = head;
	while(NULL != temp){
		if(temp->block_size >= request_size && temp->status != ALLOCATED_BLOCK){
	#ifdef DEBUG_LEVEL_1
				printf("Return from function %s value %p\n",__func__, temp);
	#endif
			return(temp);
		}
		temp = temp->next;
	}
	#ifdef DEBUG_LEVEL_1
		printf("Return from function %s value NULL\n\n",__func__);
	#endif
	return(NULL);

}



// Algorithm: cannot change the data
BlockMeta* findCompatibleBlock_best_fit(BlockMeta *head, size_t request_size)
{
	#ifdef DEBUG_LEVEL_1
		printf("In Function: %s ,line %d\n",__func__, __LINE__ );
		printf("Request to find block for %ld Bytes\n",request_size);
	#endif
	assert(head != NULL);
	assert(request_size > 0);
	BlockMeta *temp = head, *best_match = NULL;
	int minimum_diff = (int)INFINITY, diff;
	while(NULL != temp){
		if(temp->block_size >= request_size && temp->status != ALLOCATED_BLOCK){
			diff = head->block_size - request_size;
			if(diff < minimum_diff)
			{
				minimum_diff = diff;
				best_match = temp;
			}
		}
		temp = temp->next;
	}
	#ifdef DEBUG_LEVEL_1
		printf("Return from function %s value NULL\n\n",__func__);
	#endif
	return(best_match);
}




// Algorithm: cannot change the data
BlockMeta* findCompatibleBlock_worst_fit(BlockMeta *head, size_t request_size)
{
	#ifdef DEBUG_LEVEL_1
		printf("In Function: %s ,line %d\n",__func__, __LINE__ );
		printf("Request to find block for %ld Bytes\n",request_size);
	#endif
	assert(head != NULL);
	assert(request_size > 0);
	BlockMeta *temp = head, *worst_match = NULL;
	int maximum_diff = -1*(int)INFINITY, diff;
	while(NULL != temp){
		if(temp->block_size >= request_size && temp->status != ALLOCATED_BLOCK){
			diff = head->block_size - request_size;
			if(diff > maximum_diff)
			{
				maximum_diff = diff;
				worst_match = temp;
			}
		}
		temp = temp->next;
	}
	#ifdef DEBUG_LEVEL_1
		printf("Return from function %s value NULL\n\n",__func__);
	#endif
	return(worst_match);
}





// Algorithm: cannot change the data
BlockMeta* findCompatibleBlock_last_fit(BlockMeta *head, size_t request_size)
{
	#ifdef DEBUG_LEVEL_1
		printf("In Function: %s ,line %d\n",__func__, __LINE__ );
		printf("Request to find block for %ld Bytes\n",request_size);
	#endif
	assert(head != NULL);
	assert(request_size > 0);
	BlockMeta *temp ;
	if(last_allocated == NULL)
		temp = head;
	else
		temp = last_allocated;
	while(NULL != temp){
		if(temp->block_size >= request_size && temp->status != ALLOCATED_BLOCK){
	#ifdef DEBUG_LEVEL_1
				printf("a.Return from function %s value %p\n",__func__, temp);
	#endif
			last_allocated = temp;
			return temp;
		}
		temp = temp->next;
	}
	#ifdef DEBUG_LEVEL_1
		printf("b.Return from function %s value NULL\n\n",__func__);
	#endif
	return(NULL);
}





void*  get_memory_from_OperatingSystem(size_t size)
{
	#ifdef DEBUG_LEVEL_1
		printf("In Function: %s\n",__func__);
		printf("request to allocate %ld Bytes\n",size);
	#endif
	size_t requestSize = size + BLOCK_META_SIZE;
	size_t size_to_allocate = MAX(requestSize, DEFAULT_ALLOC_SIZE);
	BlockMeta *presentBreak = sbrk(0);
	void *allocatedBlock = sbrk(size_to_allocate);//(intptr_t)
	assert((void *)presentBreak == allocatedBlock);
	if((void*)-1 == allocatedBlock){
	#ifdef DEBUG_LEVEL_1
		printf("a.Return from function %s value NULL\n\n",__func__);
	#endif
		return NULL;
	}
	if(requestSize < DEFAULT_ALLOC_SIZE)/* allocate default size*/
		presentBreak->block_size = DEFAULT_ALLOC_SIZE - BLOCK_META_SIZE;
	else /*allocate maximum size*/
		presentBreak->block_size = size;
	presentBreak->next = NULL;
	#ifdef DEBUG_LEVEL_1
		printf("b.Return from function %s value NULL\n\n",__func__);
	#endif
	return(presentBreak);
}




void split(BlockMeta *before_split, size_t size)
{
	#ifdef DEBUG_LEVEL_1
		printf("In Function: %s\n",__func__);
	#endif
	if((before_split->block_size - size) > UNUSABLE_MEMORY_SIZE)/* split */
	{

		void *mem_block = BLOCK_MEM(before_split);
		BlockMeta *newptr = (BlockMeta *) ((unsigned long)mem_block + size);
		newptr->block_size = before_split->block_size - (size + BLOCK_META_SIZE);
		before_split->block_size = size;
		newptr->next = before_split->next;
		before_split->next = newptr;		
	}
	/* else don't split */
	#ifdef DEBUG_LEVEL_1
		printf("Return from function %s\n\n",__func__);
	#endif
}



void* u_malloc(size_t size)
{
	#ifdef DEBUG_LEVEL_1
		printf("In Function: %s\n",__func__);
	#endif
	assert(size > 0);
	BlockMeta *block = NULL;
	if(NULL == head){ /* this is the first request since program start*/
		head = get_memory_from_OperatingSystem(size);
		block = head;
		split(block, size);
		assert(head == block);
		block->status = ALLOCATED_BLOCK;
	}
	else /* head exists */{
		BlockMeta *compatibleBlock = NULL;
		compatibleBlock = findCompatibleBlock_last_fit(head, size);
		if(NULL != compatibleBlock){ /* requested memeory size available */
			block = compatibleBlock;
			split(block, size);
			block->status = ALLOCATED_BLOCK;
		}
		else{ /* requested memory size not available */
			BlockMeta *last_block = head;
			while(last_block->next != NULL)
				last_block = last_block->next;
			block = get_memory_from_OperatingSystem(size);
			last_block->next = block;
			split(block, size);
			block->status = ALLOCATED_BLOCK;
		}
	}
	#ifdef DEBUG_LEVEL_1
		printf("Return from function %s value NULL\n\n",__func__);
	#endif
	return (void*)(block+1);
}


void* u_realloc(void *data, size_t newSize){
	#ifdef DEBUG_LEVEL_1
		printf("In Function: %s\n",__func__);
	#endif
	BlockMeta *block = (BlockMeta*)data - 1;
    size_t originalSize = block-> block_size;
	#ifdef DEBUG_LEVEL_1
		printf("Request to realloc %ldB to %ldB\n", originalSize, newSize);
	#endif
	if (newSize <= 0){ /* return NULL */
        u_free(data);
        return NULL;
    }
    else if (data == NULL){ // call u_malloc
            return u_malloc(newSize);
         }
         else if (newSize <= originalSize){ // return without any change
                return data;
              }
              else{ assert((data) && (newSize > originalSize));
                    void *dataNew = u_malloc(newSize);
                    if (dataNew){
                        memcpy(dataNew, data, originalSize);
                        u_free(data);
                    }
					else
						return NULL;
                    return dataNew;
              }
}


void u_free(void *memptr)
{
	#ifdef DEBUG_LEVEL_1
		printf("In Function: %s\n",__func__);
	#endif
	BlockMeta *ptr = (BlockMeta*)memptr;
	BlockMeta *block_ptr = ptr-1;
	block_ptr->status = FREE_BLOCK;
	#ifdef DEBUG_LEVEL_1
		printf("Return from function %s value NULL\n\n",__func__);
	#endif
}


void travers_and_print()
{
	BlockMeta *ptr = head;
	printf("list of sizes allocated:\n");
	while(ptr!=NULL)
	{
		printf("%ld ",ptr->block_size);
		ptr = ptr->next;
	}
	printf("\n");
}