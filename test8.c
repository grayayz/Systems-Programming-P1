#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mymalloc.h"

/* mallocs different sizes and checks every pointer is 8-byte aligned */
int main( void )
{
    printf( "test8: alignment\n" );
    
    void *ptrs[20];
    int errors = 0;
    
    for ( int i = 0; i < 20; i++ )
    {
        ptrs[i] = malloc( i + 1 );
        if ( ptrs[i] == NULL )
        {
            printf( "malloc failed\n" );
            return 1;
        }
        
        if (( uintptr_t )ptrs[i] % 8 != 0 )
        {
            printf( "%p not aligned (size %d)\n", ptrs[i], i + 1 );
            errors++;
        }
    }
    
    for ( int i = 0; i < 20; i++ )
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
        printf( "fail: %d not aligned\n", errors );
        return 1;
    }
}
