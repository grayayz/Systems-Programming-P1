CS 214 Spring - Project 1: My Little Malloc

AUTHORS:
Janine Al-Zahiri - joa43
[your name] - [your netID]


**DESIGN OVERVIEW:**

--MEMORY LAYOUT--
Our implementation uses a 4096-byte heap stored in a static array. The heap is
organized as a sequence of variable-sized chunks, where each chunk consists of:
  - Header (8 bytes): Contains size and allocation status
  - Payload (variable): User-accessible memory


--HEADER STRUCTURE--
We use a compact 8-byte header that stores both size and allocation status:
  - Type: size_t (8 bytes on 64-bit systems)
  - Bit 0: Allocation flag (1 = allocated, 0 = free)
  - Bits 63-1: Chunk size in bytes

This design works because all chunk sizes are multiples of 8 (for alignment),
so bit 0 is always 0 in a valid size. We "steal" this bit to store the
allocation status, eliminating the need for a separate flag field.

Example:
  - Allocated 64-byte chunk: header = 65 (0b...01000001)
  - Free 64-byte chunk:      header = 64 (0b...01000000)

To extract:
  - Size: header & ~1UL (clear bit 0)
  - Status: header & 1 (read bit 0)

--ALIGNMENT--
All allocations are rounded up to multiples of 8 bytes to ensure proper
alignment for any data type. The minimum chunk size is 16 bytes (8-byte header
+ 8-byte minimum payload).

--CHUNK TRAVERSAL--
Chunks form an implicit linked list. To find the next chunk, we add the current
chunk's size to its address. The first chunk always starts at heap.bytes[0].

--INITIALIZATION--
The heap is initialized on the first call to malloc() or free(). Initialization:
*include description for initialization*

--MALLOC IMPLEMENTATION--
*include description for malloc implementation*

--FREE IMPLEMENTATION--
Our free() performs validation, deallocation, and coalescing:
  1. Validate that the pointer is within heap bounds
  2. Calculate chunk start (pointer - 8 bytes)
  3. Verify pointer points to a valid chunk start by traversing heap 
  4. Verify chunk is currently allocated (detect double-free)
  5. Mark chunk as free
  6. Coalesce with next chunk if it's free (forward coalescing)
  7. Coalesce with previous chunk if it's free (backward coalescing)

--COALESCING--
Forward coalescing:
  - Check if next chunk exists and is free
  - If yes, merge by adding next chunk's size to current chunk

Backward coalescing:
  - Traverse from the beginning of the heap to find previous chunk
  - If previous chunk exists and is free, expand it to include current chunk

We chose to coalesce in free() rather than malloc() to minimize fragmentation
and ensure adjacent free chunks are always merged immediately.

--ERROR DETECTION--
Our implementation detects the following errors:

in free:
1. Invalid pointer (not from malloc):
   - Check if pointer is within heap bounds [heap.bytes, heap.bytes + 4096)
   - Traverse heap to verify pointer points to a valid chunk header
2. Offset pointer (not at chunk start):
   - Same validation as above - if pointer is in heap but doesn't match any
     chunk header location, it's an offset pointer
3. Double-free:
   - Check if chunk is already marked as free
   - If yes, it's a double-free attempt
All errors print to stderr with format:
  "free: Inappropriate pointer (filename:line)"
Then call exit(2) to terminate the program.

in malloc:
1.[description of errors]


--LEAK DETECTION--
[description for leak detection]

**TEST PLAN**

--TESTING STRATEGY--
Our testing is divided into three categories:
  1. Correctness tests - Verify malloc/free work properly
  2. Error detection tests - Verify improper usage is caught
  3. Stress tests - Verify performance under load

--CORRECTNESS TESTS--

memtest (provided by instructor):
  Purpose: Verify allocations don't overlap
  Method: Allocates 64 objects filling entire heap, writes unique byte pattern
          to each, verifies no corruption
  Usage: ./memtest
  Expected output: "0 incorrect bytes"

test1 -
  Purpose:
  Method: 
  Usage: 
  Expected output: 

test2 -
  Purpose: 
  Method:      
  Usage: 
  Expected output:

test3 -
  Purpose:
  Method: 
  Usage:
  Expected output:

--ERROR DETECTION TESTS--
test4 -
  Purpose:
  Method:
  Usage:
  Expected:

test5 - Double free:
  Purpose:
  Method:
  Usage:
  Expected:

test6 -
  Purpose:
  Method:
  Usage:
  Expected:

--STRESS TESTS--

memgrind:
  Purpose: Tests performance and stress testing using realistic workloads
  Method: Runs each workload 50 times and reports average execution time
  Usage: ./memgrind
  Note: Our memgrind returns average execution time in microseconds for easier        
  readability.

Workload 1 - Immediate free:
  - malloc() and immediately free() a 1-byte object, 120 times
  - Tests: Basic malloc/free performance, minimal fragmentation

Workload 2 - Batch operations:
  - malloc() 120 1-byte objects into array, then free() all
  - Tests: Heap filling, batch deallocation, coalescing performance

Workload 3 - Random allocation/deallocation:
  - Randomly allocate or free until 120 total allocations performed
  - Deallocate all remaining objects
  - Tests: Realistic memory usage pattern, fragmentation handling

Workload 4 - Variable sizes (simulates linked list):
  - Allocate 120 chunks of varying sizes (1-10 bytes, cycling)
  - Free every other chunk (creates fragmentation)
  - Free remaining chunks
  - Tests: Variable-sized allocations, fragmentation, coalescing

Workload 5 - Tree structure (simulates binary tree):
  - Allocate root node (8 bytes)
  - Allocate 60 left children (4 bytes each)
  - Allocate 60 right children (4 bytes each)
  - Free all children, then free root
  - Tests: Hierarchical allocation pattern, deallocation order independence

**COMPILATION AND EXECUTION**

--BUILDING--
  make          - Compile all programs
  make clean    - Remove all executables and object files

--RUNNING TESTS--
  ./memtest     - Run provided correctness test
  ./test1       -
  ./test2       -
  ./test3       -
  ./test4       -
  ./test5       -
  ./test6       -
  ./memgrind    -

--EXPECTED RESULTS--
Correctness tests (memtest, test1, test2, test3):
  -

Error detection tests (test4, test5, test6):
  - 

memgrind:
  - Should output timing for all 5 workloads
  - Typical times: 10-500 microseconds per workload
  - Should exit with status 0

**CONTRIBUTIONS**

Janine:
  - Implemented myfree() with error detection and coalescing
  - Implemented helper functions: getChunkSize, setChunkSize, isAllocated, isFree,
    MarkAllocated, markFree, isValidChunkStart, coalesceNext, coalescePrev
  - Created memgrind.c
  - Created Makefile

[Your name]:
  - Implemented mymalloc()
  - Implemented heap initialization and leak detection
  - Wrote correctness test programs
  - [anything u wanna add this is just what i wrote at the top of my head]

Both partners:
  - Collaborated on header structure design
  - Joint debugging sessions
  - README documentation
  - Testing and validation

================================================================================
