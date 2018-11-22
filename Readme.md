# Usage
This driver maps user memory into the kernel and tests the affects it has on the system.

Open questions:
1) does get\_user\_pages(\_fast) pin memory (cannot be moved or swapped)
2) can i use largepages?
3) affects of multiple users

## Building
make  

## Inserting the driver
sudo insmod ukm.ko

## Generating incorrect results
Launch tester, which allocates memory and maps it into the kernel.  It then turns off preempt and locks the cpu, and constantly checks the page(s) for modifications (bytes turn from A to B).  Once the change is detected, the driver the mutates the memory from B to C.

Launch memspiker, which eats up user memory, causing the system to make use of swap.
 
sudo su

./tester &

./memspiker &


## Results
Expect pages to never be swapped (Any deadlock will mean this is untrue)
Expect pages to mutate with the useland program (A to B)
Expect pages to mutate with the kernel driver (B to C)
