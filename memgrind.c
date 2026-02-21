#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"

#define WORKLOAD_RUNS 50


long getTimeMicrosec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

//need to implement 5 workloads
void workload1(){
    //malloc() and immediately free() a 1-byte object 120 times
    for (int i = 0; i < 120; i++){
        char *p = malloc(1);
        free(p);
    }
}

void workload2(){
    //use malloc() to get 120 1-byte objects, storing the pointers in
    //array, then use free() to deallocate the chunks
    char *ptrs[120];
    for (int i = 0; i < 120; i++){
        ptrs[i] = malloc(1);
    }
    for (int i = 0; i < 120; i++){
        ptrs[i] = free(1);
    }
}

void workload3(){
    //create an array of 120 pointers. Repeatedly make a random choice between
    //(a) allocating a 1-byte object and adding the pointer to the array and 
    //(b) deallocating a random previously allocated object (if any).
    //Once 120 allocations have been performed, deallocate all remaining objects
    char *ptrs[120];
    int count = 0;
    int allocated = 0;

    while (allocated < 120){
        //random choice
        int choice = rand() % 2; //0 and 1
        if (choice == 0 || count == 0){
            //allocate
            ptrs[count] = malloc(1);
            count++
            allocated++
        } else{
            //free
            int randExists = rand() % count;
            free(ptrs[randExists]);
            //remove from array
            ptrs[randExists] = ptrs[count - 1];
            count--;
        }
    }
    //deallocate all remaining objects
    (for int i = 0; i < 120; i++){
        free(ptrs[i]);
    }
}

void workload4(){
    // U DESIGN THIS
}
void workload5(){
    //U DESIGN THIS
}

int main(){
    long startTime, endTime;
    double averageTime;

    //workload 1
    startTime = getTimeMicrosec();
    for (int i = 0; i < WORKLOAD_RUNS; i++){
        workload1();
    }
    endTime = getTimeMicrosec();
    averageTime = (endTime - startTime) / (double)WORKLOAD_RUNS;
    printf("Workload 1 Average: %.2f microseconds \n", averageTime);

    //workload 2
    startTime = getTimeMicrosec();
    for (int i = 0; i < WORKLOAD_RUNS; i++){
        workload2();
    }
    endTime = getTimeMicrosec();
    averageTime = (endTime - startTime) / (double)WORKLOAD_RUNS;
    printf("Workload 2 Average: %.2f microseconds \n", averageTime);

    //workload 3
    startTime = getTimeMicrosec();
    for (int i = 0; i < WORKLOAD_RUNS; i++){
        workload3();
    }
    endTime = getTimeMicrosec();
    averageTime = (endTime - startTime) / (double)WORKLOAD_RUNS;
    printf("Workload 3 Average: %.2f microseconds \n", averageTime);   
    
    
    //workload 4
    startTime = getTimeMicrosec();
    for (int i = 0; i < WORKLOAD_RUNS; i++){
        workload4();
    }
    endTime = getTimeMicrosec();
    averageTime = (endTime - startTime) / (double)WORKLOAD_RUNS;
    printf("Workload 4 Average: %.2f microseconds \n", averageTime);

    //workload 5
    startTime = getTimeMicrosec();
    for (int i = 0; i < WORKLOAD_RUNS; i++){
        workload5();
    }
    endTime = getTimeMicrosec();
    averageTime = (endTime - startTime) / (double)WORKLOAD_RUNS;
    printf("Workload 5 Average: %.2f microseconds \n", averageTime);
}
