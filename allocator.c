#include <stdio.h>
#include <windows.h>

struct Header {
    size_t size;
    int is_free; // 0 if the memory block is allocated, 1 if it is free to use
    struct Header *next;
};

struct Header *free_list_head = NULL;

void heap_init()
{
    int total_size = 2 * 1024 * 1024; // 2 MB

    void *raw_memory = VirtualAlloc(
        NULL,
        total_size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
     );

     if(raw_memory == NULL) {
         fprintf(stderr, "VirtualAlloc failed: %lu\n", GetLastError());
         return;
     }

    struct Header *current = (struct Header *)raw_memory; // Type casting the raw chunk of memory to a Header pointer
    
    // Filling data in the header
    current->size = total_size;
    current->is_free = 1;
    current->next = NULL;

    free_list_head = current; // Setting the head of the free list to the first block
}

struct Header *find_memory(size_t size)
{
   struct Header *current = free_list_head;

   while(current) {
    if(current->is_free == 1 && current->size >= size) {
        return current;
    } 
    current = current->next;
   }
   return NULL; // No suitable block found
}

void split_memory(struct Header *block, size_t size) {
    //Forming a new block by carving the original block
      struct Header *new_block = (struct Header *)((char *)block + sizeof(struct Header) + size);
      new_block->is_free = 1;
      new_block->size = (block->size - size) - (sizeof(struct Header));
      new_block->next = block->next;

      // reconfiguring the original block
      block->next = new_block;
      block->size = size + (sizeof(struct Header));
      block->is_free = 0;
}