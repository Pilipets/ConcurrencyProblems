Recently, I encountered a problem with finding easy-to-use and easy-to-understand neat atomic/lock-free/concurrent data structures and solutions to common concurrency problems using C++20.

This repository is a solution to the above-described problems and provided for educational purposes:
- Atomic/Lock-free/Concurrent data-structures: stack, queue, spinlock;
- Concurrency patterns: Multiplex, Barrier, SignalQueue, LightSwitch;
- Performance tests of ThreadSafe data structures implementations;

TODO:
- Add concurrent stack implementation using hazard pointers;
- Add more implmentations of the concurrent data structures without memory leaks;
- Add more common concurrency problems mentioned here - https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf;

Used links:
- https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf;
- https://www.bogotobogo.com/cplusplus/files/CplusplusConcurrencyInAction_PracticalMultithreading.pdf;
- https://www.cs.rochester.edu/research/synchronization/pseudocode/queues.html;
- https://rigtorp.se/spinlock/;