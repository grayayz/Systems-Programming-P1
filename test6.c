#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

/* frees ptr+5 instead of ptr to see if we catch freeing the middle of a chunk */
int main( void )
{
    printf( "test6: free middle of chunk\n" );
    
    int *ptr = ( int * )malloc( sizeof( int ) * 10 );
    if ( ptr == NULL )
    {
        printf( "malloc failed\n" );
        return 1;
    }
    
    free( ptr + 5 );
    
    printf( "should have caught bad pointer\n" );
    free( ptr );
    return 1;
}
