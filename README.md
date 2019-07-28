# LinuxMemoryAllocator
Linux memory allocator from scratch.
This project implements malloc, calloc, realloc, free in C. 

Key points:
1. Have block metadata and linked list of metadata structures
2. Uses sbrk() system call for low size and mmap() for size above a threshold
3. free unmarks the block and combines adjacent blocks. 
4. A request invokes the pipeline roughly 'linked list search-> split/request to os-> return block'
5. A free invokes roughly the pipeline 'mark block -> merge blocks -> return'<br>
references: Doug Lea Malloc http://gee.cs.oswego.edu/dl/html/malloc.html <br>
            http://danluu.com/malloc-tutorial/
