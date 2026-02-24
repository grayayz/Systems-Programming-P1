#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

/* frees a stack variable so we can check that free catches bad pointers */
int main( void )
{
    printf( "test4: free stack var\n" );
    
    int x = 42;
    free( &x );
    
    printf( "should have caught that\n" );
    return 1;
}
