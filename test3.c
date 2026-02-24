#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

/* frees a bunch of small chunks then tries to grab one big one to see if coalescing works */
int main( void )
{
    printf( "test3: coalesce\n" );
    
    void *ptrs[10];
    for ( int i = 0; i < 10; i++ )
    {
        ptrs[i] = malloc( 24 );
        if ( ptrs[i] == NULL )
        {
            printf( "malloc failed\n" );
            return 1;
        }
    }
    
    for ( int i = 0; i < 10; i++ )
    {
        free( ptrs[i] );
    }
    
    void *large = malloc( 200 );
    if ( large == NULL )
    {
        printf( "couldn't get big chunk after freeing small ones\n" );
        return 1;
    }
    
    free( large );
    
    printf( "ok\n" );
    return 0;
}
