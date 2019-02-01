# Info

The get\_user\_pages driver demonstrates the useage of that API.
Additionally, demonstrate that these pages will never be swapped.
This interface is GPL'd, and should not be used in proprietary
software.

The vmap driver demonstrates how to remap 2 separately allocated
kmalloc pages into a single virtually contiguous block, and
demonstrate that these pages will never be swapped.

The mmap driver demonstrates how to allow user programs to allocate
memory backed my pages allocated by the driver.

The shared\_memory driver demonstrates a combination of vmap and
mmap driver, such that a virtually contiguous range of memory
can be shared between user and kernel mode in a friendly manner.
