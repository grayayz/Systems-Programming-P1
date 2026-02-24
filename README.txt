CS 214 Spring - Project 1: My Little Malloc

AUTHORS:
Janine Al-Zahiri - joa43
Antonio Pichardo - ap2694


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
1. We catch three main mistakes in free: 
  - pointers not from our heap, pointers that don’t point to the start of a block, and double frees. 
    - On each free, we scan the heap’s chunk headers to find a matching allocated block; if it’s missing, already free, 
      or the pointer lands in the middle of a chunk,  we print an error (with __FILE__/__LINE__) and exit with status 2.


--LEAK DETECTION--
we register a leak checker with atexit so it runs automatically when the program ends. 
It walks the heap, totals up any chunks still marked allocated, and prints how many objects and bytes were leaked (or that there were no leaks), 
without the user having to call anything special.

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
  Purpose: Check that back‑to‑back malloc calls give separate, non‑overlapping blocks.
  Method: Allocate many small blocks, fill each with a different pattern, then verify every block still has its own pattern and distinct address.
  Usage: ./test1
  Expected output: runs to completion with no errors and exits with status 0.
  test2 -
  Purpose: Make sure freeing a block lets us reuse that space.
  Method: Allocate A and B, free A, then allocate C of A’s size and confirm C’s pointer matches A and can be read/written safely.
  Usage: ./test2
  Expected output: Confirms reuse of A’s address (or otherwise reports success) and exits with status 0.
  test3 -
  Purpose: Verify that freeing neighbors lets us coalesce and satisfy a larger request.
  Method: allocate A, B, C; free A and B; then request one larger block about size A+B and check it fits and starts where A did.
  Usage: ./test3
  Expected output: Reports that the big allocation succeeded using the coalesced space and exits with status 0.
  --ERROR DETECTION TESTS--
  test4 -
  Purpose: Confirm freeing a non‑malloc pointer (like a stack variable) is rejected.
  Method: Take the address of a local variable, call free on it, and rely on the heap scan to fail to find a matching chunk.
  Usage: ./test4
  Expected: Prints an invalid‑pointer free error with file/line and exits with status 2.
  test5 - Double free:
  Purpose: Make sure calling free twice on the same pointer is caught.
  Method: Allocate a block, free it once, then free it again so the allocator sees the chunk is already free and flags it.
  Usage: ./test5
  Expected: Prints a double‑free error with file/line and exits with status 2.
  test6 -
  Purpose: Catch frees of a pointer into the middle of a block.
  Method: Allocate a block, then call free on ((char *)ptr + offset) where offset is nonzero but within the block.
  Usage: ./test6
  Expected:å Prints an error about freeing the middle of a chunk (invalid pointer) with file/line and exits with status 2.

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
  ./test1       - Verifies malloc() reserves non-overlapping memory
  ./test2       - Verifies free() deallocates memory for reuse
  ./test3       - Verifies coalescing of adjacent free blocks
  ./test4       - Tests error detection for invalid pointer (stack variable)
  ./test5       - Tests error detection for double free
  ./test6       - Tests error detection for freeing middle of chunk
  ./memgrind    - Stress test with five workloads; reports timing

-EXPECTED RESULTS--
Correctness tests (memtest, test1, test2, test3):
  - Exit with status 0; may print a short success message.

Error detection tests (test4, test5, test6):
  - Print an error message (file and line) and exit with status 2.

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

Antonio:
  - Implemented mymalloc()
  - Implemented heap initialization and leak detection
  - Wrote correctness test programs
  - Wrote error test programs

Both partners:
  - Collaborated on header structure design
  - Joint debugging sessions
  - README documentation
  - Testing and validation
