#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

#define HEADER_SIZE 8
#define MIN_CHUNK_SIZE 16
#define MEMLENGTH 4096
#define ALIGNMENT 8

static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;

typedef struct {
    size_t size;
} chunkHeader;

static int heapInitialized = 0;

static size_t alignSize( size_t size ) 
{
    return (( size + ALIGNMENT - 1 ) & ~( ALIGNMENT - 1 ));
}

static size_t getChunkSize( char *chunk ) 
{
    chunkHeader *header = ( chunkHeader * )chunk;
    return header->size & ~1UL;
}

static void setChunkSize( char *chunk, size_t size ) 
{
    chunkHeader *header = ( chunkHeader * )chunk;
    header->size = ( size & ~1UL ) | ( header->size & 1 );
}

static int isAllocated( char *chunk ) 
{
    chunkHeader *header = ( chunkHeader * )chunk;
    return header->size & 1;
}

static int isFree( char *chunk ) 
{
    return !isAllocated( chunk );
}

static void markFree( char *chunk ) {
    chunkHeader *header = ( chunkHeader * )chunk;
    header->size &= ~1UL;
}

static void markAllocated( char *chunk ) 
{
    chunkHeader *header = ( chunkHeader * )chunk;
    header->size |= 1;
}

static int isValidChunkStart( char *chunkStart ) 
{
    char *current = heap.bytes;
    while ( current < heap.bytes + MEMLENGTH ) 
    {
        if ( current == chunkStart ) 
        {
            return 1;
        }
        size_t chunkSize = getChunkSize( current );
        if ( chunkSize == 0 ) 
        {
            break;
        }
        current += chunkSize;
    }
    return 0;
}

static void coalesceNext( char *chunkStart ) {
    size_t currentSize = getChunkSize( chunkStart );
    char *nextChunk = currentSize + chunkStart;
    if ( nextChunk >= heap.bytes + MEMLENGTH ) {
        return;
    }
    if ( isFree( nextChunk )) {
        size_t nextChunkSize = getChunkSize( nextChunk );
        size_t newChunkSize = currentSize + nextChunkSize;
        setChunkSize( chunkStart, newChunkSize );
    }
}

static void coalescePrev( char *chunkStart ) {
    if ( chunkStart == heap.bytes ) {
        return;
    }
    char *current = heap.bytes;
    char *prev = NULL;
    while ( current < chunkStart ) {
        prev = current;
        size_t size = getChunkSize( current );
        current += size;
    }
    if ( prev != NULL && isFree( prev )) {
        size_t prevSize = getChunkSize( prev );
        size_t currentSize = getChunkSize( chunkStart );
        size_t newChunkSize = prevSize + currentSize;
        setChunkSize( prev, newChunkSize );
    }
}

// this will scan the heap and report any allocated chunks that weren't freed
static void checkLeaks( void ) {
    char *current = heap.bytes;
    size_t total_leaked = 0;
    int num_objects = 0;
    
    while ( current < heap.bytes + MEMLENGTH ) {
        size_t chunkSize = getChunkSize( current );
        if ( chunkSize == 0 ) {
            break;
        }
        if ( isAllocated( current )) {
            size_t payload_size = chunkSize - HEADER_SIZE;
            total_leaked += payload_size;
            num_objects++;
        }
        current += chunkSize;
    }
    
    if ( total_leaked > 0 ) {
        fprintf( stderr, "mymalloc: %zu bytes leaked in %d objects.\n", 
                total_leaked, num_objects );
    }
}

// heap and leak detector
static void initializeHeap( void ) {
    chunkHeader *header = ( chunkHeader * )heap.bytes;
    header->size = MEMLENGTH;
    markFree( heap.bytes );
    heapInitialized = 1;
    atexit( checkLeaks );
}

static char *findFreeChunk( size_t size ) {
    char *current = heap.bytes;
    while ( current < heap.bytes + MEMLENGTH ) 
    {
        size_t chunkSize = getChunkSize( current );
        if ( chunkSize == 0 ) {
            break;
        }
        if ( isFree( current ) && chunkSize >= size ) {
            return current;
        }
        current += chunkSize;
    }
    return NULL;
}

static void splitChunk( char *chunk, size_t size ) {
    size_t chunkSize = getChunkSize( chunk );
    if ( chunkSize - size >= MIN_CHUNK_SIZE ) 
    {
        char *newChunk = chunk + size;
        setChunkSize( newChunk, chunkSize - size );
        markFree( newChunk );
        setChunkSize( chunk, size );
    }
}

void *mymalloc( size_t size, char *file, int line ) {
    if ( !heapInitialized ) 
    {
        initializeHeap();
    }
    
    size_t aligned_size = alignSize( size );
    size_t total_size = HEADER_SIZE + aligned_size;
    
    if ( total_size < MIN_CHUNK_SIZE ) {
        total_size = MIN_CHUNK_SIZE;
    }
    
    total_size = alignSize( total_size );
    
    char *chunk = findFreeChunk( total_size );
    
    // if there's no chunk found,  coalescing and searching again
    if ( chunk == NULL ) 
    {
        char *coalesce_chunk = heap.bytes;
        while ( coalesce_chunk < heap.bytes + MEMLENGTH ) 
        {
            size_t chunk_size = getChunkSize( coalesce_chunk );
            if ( chunk_size == 0 ) {
                break;
            }
            if ( isFree( coalesce_chunk )) {
                coalesceNext( coalesce_chunk );
                coalescePrev( coalesce_chunk );
                chunk_size = getChunkSize( coalesce_chunk );
            }
            coalesce_chunk += chunk_size;
        }
        
        chunk = findFreeChunk( total_size );
    }
    
    if ( chunk == NULL ) 
    {
        fprintf( stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", 
                size, file, line );
        return NULL;
    }
    
    splitChunk( chunk, total_size );
    
    markAllocated( chunk );
    
    return chunk + HEADER_SIZE;
}

void myfree( void *ptr, char *file, int line ) {
    if ( !heapInitialized ) 
    {
        initializeHeap();
    }
    
    if ( ptr == NULL ) 
    {
        return;
    }
    
    if (( char * )ptr < heap.bytes || ( char * )ptr >= heap.bytes + MEMLENGTH ) 
    {
        fprintf( stderr, "free: Inappropriate pointer (%s:%d)\n", file, line );
        exit( 2 );
    }
    
    char *chunkStart = ( char * )ptr - HEADER_SIZE;
    
    if ( !isValidChunkStart( chunkStart )) 
    {
        fprintf( stderr, "free: Inappropriate pointer (%s:%d)\n", file, line );
        exit( 2 );
    }
    
    if ( isFree( chunkStart )) 
    {
        fprintf( stderr, "free: Inappropriate pointer (%s:%d)\n", file, line );
        exit( 2 );
    }
    
    markFree( chunkStart );
    coalesceNext( chunkStart );
    coalescePrev( chunkStart );
}
