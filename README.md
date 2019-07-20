# LinuxMemoryAllocator
Linux memory allocator from scratch


Trying to implement malloc, calloc, realloc, free here. 

Key points:
1. Have block metadata and linked list of metadata structures
2. sbrk for low size and mmap for size above a threshold
3. free will mark the block and will combine. 
4. a request will invoke the pipeline roughly 'linked list search-> split/request to os-> return block'
5. a free will invoke roughly the pipeline 'mark block -> merge blocks -> return'<br>
references: Doug Lea Malloc http://gee.cs.oswego.edu/dl/html/malloc.html <br>
            http://danluu.com/malloc-tutorial/
