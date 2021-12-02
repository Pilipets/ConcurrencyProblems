Recently, I encountered a problem with finding easy-to-use and easy-to-understand neat atomic/lock-free/concurrent data structures and solutions to common concurrency problems using C++20.

This repository is a solution to the above-described problems:
- Atomic/Lock-free/Concurrent data-structures: stacks, queues, spinlocks;
- Concurrency patterns: Multiplex, Barrier, SignalQueue, LightSwitch;
- Performance tests of different thread-safe data structures implementations;

I'm not claiming that implementations here are the most efficient ones since they are provided for educational purposes.

TODO:
- Add hazard pointers implementation;
- Add more implmentations of the concurrent data structures;
- Add more common concurrency problems;

Used links:
- https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf;
- https://www.bogotobogo.com/cplusplus/files/CplusplusConcurrencyInAction_PracticalMultithreading.pdf;
- https://www.cs.rochester.edu/research/synchronization/pseudocode/queues.html;
- https://rigtorp.se/spinlock/;