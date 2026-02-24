#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

/* frees the same pointer twice to see if we catch double free */
int main( void )
{
    printf( "test5: double free\n" );
    
    void *ptr = malloc( 100 );
    if ( ptr == NULL )
    {
        printf( "malloc failed\n" );
        return 1;
    }
    
    free( ptr );
    free( ptr );
    
    printf( "should have caught double free\n" );
    return 1;
}
