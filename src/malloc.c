/* file : malloc.c
 * description : implements malloc system call and other necessary functions
 * acknowledgements :
 * Author : 
 * Date Created:
 * Revisions :
 */

#include "../include/common.h"
#include "../include/commonFunctions.h"
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
 *
 *
 * Description
 * -----------
 *  Implements malloc, calloc, realloc and free. malloc uses sbrk 
 *  to extend the heap for small requests and if the request is more than 
 *  a threshold, mmap will be used.
 */


/* Global variable */
static BlockMeta *head = NULL;
BlockMeta *last_allocated = NULL; /*used in last fit scheme*/
static ZoneInfo *head_mmap = NULL;



/* function : findCompatibleBlock_first_fit
 * Description : scans through the linked list of blocks and see if a suitable block exists.
 * 		if so, returns the address of the block. else NULL is returned. 
 * input    : pointer to the first block, size of request
 * Author   : Shahan. 
 * */

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


/* function : findCompatibleBlock_best_fit
 * Description: using best fit strategy, finds a free block to be used from the list of already allocated blocks
 * Input : the pointer to the first block , size to be allocated. 
 * Author : Manish Kumar
 */
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



/* function : findCompatibleBlock_worst_fit 
 * Description: using worst fit strategy, tries to find a block which is free from the already allocated list 
 * 		of blocks.
 * Input : pointer to the first block, requested size to be allocated. 
 * Author : Manish Kumar
 */

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




/* function : findCompatibleBlock_last_fit
 * Description: using last fit strategy, finds a compatible free block from the already existing linked
 * 		list of blocks. 
 * Input : pointer to the first block, size to be allocated. 
 * Author : Manish Kumar
 */

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


/* function : findCompatibleZone
 * Description: from the zones which are already allocated and are presently free, this routine tries to find
 * 	       a zone which can be allocated. uses first fit. 
 * Input : pointer to first zone in the linked list, size. 
 * Author : Madhumita.
 */
ZoneInfo* findCompatibleZone(ZoneInfo *head, size_t request_size)
{
	assert(head != NULL);
	assert(request_size >= DEFAULT_MMAP_THRESHOLD);
	ZoneInfo *temp = head;
	while(NULL != temp){
		if(temp->block_size >= request_size && temp->status != ALLOCATED_ZONE){
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

/* function : get_memory_from_OperatingSystem
 * Description: using sbrk, the programs present break is extended. used for lesser sizes of memory allocation 
 *  		using malloc. 
 * Input : size to be allocated
 * Author : Madhumita
 */
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
		printf("b.Return from function %s value %p\n\n",__func__,(void *)presentBreak);
	#endif
	return(presentBreak);
}

/* function : get_memory_from_os_mmap
 * Description: allocates memory using mmap. the allocated memory is termend zone.
 * Input : size to be allocated
 * Author : Shahan.
 */
void* get_memory_from_os_mmap(size_t size)
{
	assert(size >= DEFAULT_MMAP_THRESHOLD);
	size_t request_size = size + sizeof(ZoneInfo);
	void *allocatedBlock = MMAP(0, request_size, PROT_READ | PROT_WRITE, 0);
	ZoneInfo *zoneMetaData = allocatedBlock;
	if(MAP_FAILED == allocatedBlock)
		return(NULL);
	else{
		zoneMetaData->block_size = size;
		zoneMetaData->next = NULL;
		zoneMetaData->status = ALLOCATED_ZONE;
	}
	return(zoneMetaData);
}


/* function : split
 * Description: splits a block in case there is excess size available. 
 * Input : pointer to the block, size requested
 * Author : Manish Kumar.
 */
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



/* function : u_malloc
 * Description: allocates a block of memory. when the requested size is small, sbrk is used. 
 * 	        when a considerable size greater than a threshold is requested, mmap will be used. 
 * 	        keeps track of the allocated blocks using a linked list.
 * Input : size to be allocated. 
 * Author : Shahan.
 */
void* u_malloc(size_t size)
{
	#ifdef DEBUG_LEVEL_1
	printf("In Function: %s\n",__func__);
	#endif
	if(size <= 0)
		return(NULL);
	if(size < DEFAULT_MMAP_THRESHOLD){
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
				if(NULL == block)
					return(NULL);
				last_block->next = block;
				split(block, size);
				block->status = ALLOCATED_BLOCK;
			}
		}
	return (void*)(block+1);
	}
	else{
		//allocate or reuse memory using mmap. 
		ZoneInfo *zone = NULL;
		if(NULL == head_mmap){ /*first mmap since program start*/
			head_mmap = get_memory_from_os_mmap(size);
			assert(NULL != head_mmap);
			zone = head_mmap;
			zone->status = ALLOCATED_ZONE;
		}
		else{
			ZoneInfo *compatibleZone = NULL;
			compatibleZone = findCompatibleZone(head_mmap, size);
			if(NULL != compatibleZone){
				zone = compatibleZone;
				zone->status = ALLOCATED_ZONE;
			}
			else{
				ZoneInfo *last_zone = head_mmap;
				while(last_zone->next != NULL)
					last_zone = last_zone->next;
				zone = get_memory_from_os_mmap(size);
				assert(NULL != zone);
				last_zone->next = zone;
				zone->status = ALLOCATED_ZONE;
			}
		}

	return(void *)(zone+1);
	}
}


/* function : u_calloc
 * Description: allocates and sets the memory to zero 
 * Input : number of elements
 * Author : Shahan
 */
void * u_calloc(size_t nmemb, size_t size)
{
#ifdef DEBUG_LEVEL_1
	printf("In function %s\n",__func__);
#endif
	size_t alloc_size = nmemb * size;
	void *mem_ptr = u_malloc(alloc_size);
	if(NULL == mem_ptr)
		return(NULL);
	memset(mem_ptr, 0, size);
#ifdef DEBUG_LEVEL_1
	printf("Return from function %s\n", __func__);
#endif
	return(mem_ptr);
}


/* function : u_realloc
 * Description: reallocates a memory block 
 * Input : pointer to the memory block, new size
 * Author : Manish Kumar
 */
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


/* function : u_free
 * Description: frees an allocated block 
 * Input : pointer to the memory location 
 * Author : Shahan 
 */

void u_free(void *memptr)
{
	#ifdef DEBUG_LEVEL_1
		printf("In Function: %s\n",__func__);
	#endif
	if(NULL != memptr){
		BlockMeta *ptr = (BlockMeta*)memptr;
		BlockMeta *block_ptr = ptr-1;
		block_ptr->status = FREE_BLOCK;
		//TODO : combine bocks and sbrk with negetive memory size to actually free.
#ifdef DEBUG_LEVEL_1
		printf("freed %ld Bytes\n\n",block_ptr->block_size);
#endif
	}
}


/* function : travers_and_print
 * Description: prints the blocks one by one
 * Input : none
 * Author : Manish Kumar
 */

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
