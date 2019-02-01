# Usage
This driver allocates 4MB worth of 4kb pages with kmalloc.
It then utilizes the vmap call to map a virtually contiguous
space such that a 4MB structure stored on the page range
can be accessed with ease.

Open questions:
1) Do vmap mappings survive memory pressure

## Building
make  

## Inserting the driver
sudo insmod ukm.ko

## Generating incorrect results
Launch tester, which calls the driver to kmalloc and vmap memory.
It then sends an IOCTL to begin testing access.

During this access, it will access the vmap'd pages for about 
1,000,000 iterations and return to the user process, where it will
sleep for a second or two before repeating.

During access, preempt and interrupts are turned off, to ensure
any page fault will cause a kernel segfault.

Launch memspiker, which eats memory, causing the system to make use of swap.
 
sudo su
./tester &
./memspiker &


## Results
Expect pages to never be swapped (Any segfault proves this wrong)
