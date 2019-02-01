# Info

This is a collection of drivers that demonstrate how to allocate
memory differently in the linux kernel.  Additionally, some drivers
also demonstrate how to share those virtual mappings between user
and kernel.

The get\_user\_pages driver demonstrates the useage of that API.
Additionally, demonstrate that these pages will never be swapped.
This interface is GPL'd, and should not be used in proprietary
software.

The vmap driver demonstrates how to remap 2 separately allocated
pages (using alloc\_page) into a single virtually contiguous block,
and demonstrate that these pages will never be swapped.

The vmalloc driver demonstrates how to get (struct page\*) from
a vmalloc allocation, so that it can be used with mmap.

The mmap driver demonstrates how to allow user programs to allocate
memory backed by pages allocated by the driver with alloc\_struct.
You can even replace existing mappings with driver memory if you want
to avoid additional memory dererefences.  

The shared\_memory driver demonstrates a combination of vmap and
mmap driver, such that a virtually contiguous range of memory
can be shared between user and kernel mode in a friendly manner.
