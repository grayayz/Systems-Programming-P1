#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

/* allocates and never frees so the leak detector should complain at exit */
int main( void )
{
    printf( "test7: leak\n" );
    
    void *ptr1 = malloc( 100 );
    void *ptr2 = malloc( 200 );
    void *ptr3 = malloc( 50 );
    
    if ( ptr1 == NULL || ptr2 == NULL || ptr3 == NULL )
    {
        printf( "malloc failed\n" );
        return 1;
    }
    
    return 0;
}
