Custom implementation of C malloc() and free() using best-fit policy and tests.
=======
# C-Memory-Allocator
Implementation of the malloc() and free() functions for C using best-fit allocation

# Components of Project 
vm.h has the internal data structures of the heap defined, containing all the block headers and block footers.
vminit.c contains functions that set up the simulated heap (particularly through the init_heap() function)


# Makefile and running tests
Run "make" to generate libvm.so (dynamically linked library, linked to vmtest). 

CD into tests/ and run make to compile all test programs
./vmtest will print the contents of an empty heap (for visualization sake)
./makefile will create all files needed to run the code and test cases
To test each test case, use ls when in the tests/ directory and use
./test-name
to run each test.
