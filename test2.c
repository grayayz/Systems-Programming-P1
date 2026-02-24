#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

/* frees something then mallocs again to see if we actually get that memory back */
int main( void )
{
    printf( "test2: free then reuse\n" );
    
    void *ptr1 = malloc( 200 );
    if ( ptr1 == NULL )
    {
        printf( "malloc failed\n" );
        return 1;
    }
    memset( ptr1, 0xAA, 200 );
    
    void *ptr2 = malloc( 200 );
    if ( ptr2 == NULL )
    {
        printf( "malloc failed\n" );
        return 1;
    }
    memset( ptr2, 0xBB, 200 );
    
    free( ptr1 );
    
    void *ptr3 = malloc( 200 );
    if ( ptr3 == NULL )
    {
        printf( "couldn't reuse after free\n" );
        return 1;
    }
    
    memset( ptr3, 0xCC, 200 );
    
    unsigned char *data2 = ( unsigned char * )ptr2;
    int errors = 0;
    for ( int i = 0; i < 200; i++ )
    {
        if ( data2[i] != 0xBB )
        {
            printf( "ptr2 got corrupted\n" );
            errors++;
            break;
        }
    }
    
    free( ptr2 );
    free( ptr3 );
    
    if ( errors == 0 )
    {
        printf( "ok\n" );
        return 0;
    }
    else
    {
        printf( "fail\n" );
        return 1;
    }
}
