#include "common.h"

void* u_malloc(size_t size);
void u_free(void *memptr);
void* u_calloc(size_t nmemb, size_t size);
void split(BlockMeta*, size_t);
int max(int, int);
BlockMeta* findCompatibleBlock_first_fit(BlockMeta*, size_t);
BlockMeta* findCompatibleBlock_best_fit(BlockMeta*, size_t);
BlockMeta* findCompatibleBlock_worst_fit(BlockMeta*, size_t);
BlockMeta* findCompatibleBlock_last_fit(BlockMeta*, size_t);
void*  get_memory_from_OperatingSystem(size_t);
void* u_realloc(void*, size_t);
void travers_and_print(void);
