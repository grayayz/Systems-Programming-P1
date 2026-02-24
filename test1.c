#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

/* allocates a bunch of stuff, fills each with different bytes, checks nobody overwrote anybody else */
int main( void )
{
    printf( "test1: no overlap\n" );
    
    int num_objects = 10;
    size_t obj_size = 100;
    void *ptrs[num_objects];
    
    for ( int i = 0; i < num_objects; i++ )
    {
        ptrs[i] = malloc( obj_size );
        if ( ptrs[i] == NULL )
        {
            printf( "malloc failed\n" );
            return 1;
        }
    }
    
    for ( int i = 0; i < num_objects; i++ )
    {
        memset( ptrs[i], ( char )( i + 1 ), obj_size );
    }
    
    int errors = 0;
    for ( int i = 0; i < num_objects; i++ )
    {
        char *data = ( char * )ptrs[i];
        for ( size_t j = 0; j < obj_size; j++ )
        {
            if ( data[j] != ( char )( i + 1 ))
            {
                printf( "object %d overwritten at %zu\n", i, j );
                errors++;
                break;
            }
        }
    }
    
    for ( int i = 0; i < num_objects; i++ )
    {
        free( ptrs[i] );
    }
    
    if ( errors == 0 )
    {
        printf( "ok\n" );
        return 0;
    }
    else
    {
        printf( "fail: %d corrupted\n", errors );
        return 1;
    }
}
