# Usage
This tester allocates 2MB of memory with malloc/aligned\_alloc.

Tester requests the driver to allocate 1MB of memory using alloc\_page.
The driver allocates and maps that memory using vmap, so it is 
readable from the kernel's perspective.

Tester then re-maps the second half the allocated area with the
driver allocated pages.

Tester then mutates that memory space to contain 'A'
Tester requests that the driver verify the change.
Driver verifies the memory space contains 'A' and returns the result.
Tester requests that the driver mutate memory.
Driver mutates memory to contain 'B'.
Tester verifies the memory space contains 'B'.

Tester then unmaps the 1MB of replaced memory.
Tester then mmaps zero pages into the unmapped area.
Tester then frees the space.

## Building
make  

## Inserting the driver
sudo insmod ukm.ko

## Generating incorrect results

This should not be possible, since allocations done with alloc\_page
cannot be swapped.

## Results
Smooth allocation, checks, and cleanup
