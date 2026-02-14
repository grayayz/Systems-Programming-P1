
#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

void myfree(void *ptr, char *file, int line){
     //Check if pointer is NULL
     if (ptr == NULL){
          return;
     }
     //Check if pointer is within heap bounds
     if ((char*)ptr < heap.bytes || (char*)ptr >= heap.bytes + MEMLENGTH){
          fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
          exit(2);
     }
     //Check if pointer points to the start of a valid allocated chunk
     //Check if free() is being called twice
     //if any: Stderr print: "free: Inappropriate pointer (source.c:1000)" and return exit(2)
     //need to find the header first
     char *chunkStart = (char*)ptr - HEADER_SIZE; //HEADER_SIZE depends on size of header created in mymalloc()
     //declare that chunk is no longer allocated
     if (!validStartChunk){
          fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n, file, line");
          exit(2);
     }
     if (isFree){
          fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n, file, line");
          exit2;
     }
     makeFree(chunkStart);
     coalesceNext(chunkStart);
     coalescePrev(chunkStart);
}