/*
ChunkHeader header;
header.size = 64;     // Free chunk, 64 bytes: 0...01000000

// Mark as allocated:
header.size = 65;     // Allocated, 64 bytes: 0...01000001 ->>>allocated bc it ends in 1

// Extract actual size:
actual_size = header.size & ~1;  // = 64 (clear bit 0)

// Check if allocated:
is_allocated = header.size & 1;   // = 1 (read bit 0)*/

/*
NEED TO IMPLEMENT LEAK DETECTION
NEED TO IMPLEMENT initializeHeap FUNCTION
*/


#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

#define HEADER_SIZE 8
#define MIN_CHUNK_SIZE 16
#define MEMLENGTH 4096

static union {
char bytes[MEMLENGTH];
double not_used;
} heap;

typedef struct{
     size_t size; //bit 0 = allocated (1) or free (0), rest = size
} chunkHeader;

static int heapInitialized = 0;

static size_t getChunkSize(char *chunk){
     chunkHeader *header = (chunkHeader*)chunk;
     return header->size & ~1UL;
}

static void setChunkSize(char *chunk, size_t size){
     chunkHeader *header = (chunkHeader*) chunk;
     header->size = (size & ~1UL) | (header->size & 1);
}

static int isAllocated(char *chunk){
     chunkHeader *header = (chunkHeader*)chunk;
     return header->size & 1; //bit 0
}

static int isFree(char *chunk){
     return !isAllocated(chunk);
}

static void markFree(char *chunk){
     chunkHeader *header = (chunkHeader*) chunk;
     header->size &= ~1UL;
}

static void markAllocated(char *chunk){
     chunkHeader *header = (chunkHeader*) chunk;
     header->size |= 1;
}

static int isValidChunkStart(char *chunkStart){
     char *current = heap.bytes;
     while (current < heap.bytes + MEMLENGTH){
          if (current == chunkStart){
               return 1; //found
          }
          size_t chunkSize = getChunkSize(current);
          if (chunkSize == 0){
               break;
          }
          current += chunkSize; //move to the next chunk
     } 
     return 0;
}

static void coalesceNext(char *chunkStart){
     size_t currentSize = getChunkSize(chunkStart); //get current size of chunk
     char *nextChunk = currentSize + chunkStart;
     if (nextChunk >= heap.bytes + MEMLENGTH){ 
          return; //check if it exists (& within bounds), return if it doesnt
     } 
     if (isFree(nextChunk)){ //check if its free
          //implement coalescing
          size_t nextChunkSize = getChunkSize(nextChunk);
          size_t newChunkSize = currentSize + nextChunkSize;
          setChunkSize(chunkStart, newChunkSize);
     }
}

static void coalescePrev(char *chunkStart){
     if (chunkStart == heap.bytes){
          return;
     }
     char *current = heap.bytes;
     char *prev = NULL;
     while (current < chunkStart){ //traverse through the beginning of the heap to find the chunk b4 chunkStart
          prev = current;
          size_t size = getChunkSize(current);
          current += size;
     }
     if (prev != NULL && isFree(prev)){
          size_t prevSize = getChunkSize(prev);
          size_t currentSize = getChunkSize(chunkStart);
          size_t newChunkSize = prevSize + currentSize;
          setChunkSize(prev, newChunkSize);
     }

}


void myfree(void *ptr, char *file, int line){
     if (!(heapInitialized)){
          initializeHeap(); //need a function to initialize heap
     }
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
     char *chunkStart = (char*)ptr - HEADER_SIZE; //8 bytes before the payload
     //declare that chunk is no longer allocated
     if (!isValidChunkStart(chunkStart)){
          fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
          exit(2);
     }
     if (isFree(chunkStart)){
          fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
          exit(2);
     }
     markFree(chunkStart);
     coalesceNext(chunkStart);
     coalescePrev(chunkStart);
}
