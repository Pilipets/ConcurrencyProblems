Recently, I encountered a problem with finding easy-to-use and easy-to-understand neat atomic/lock-free/concurrent data structures and solutions to common concurrency problems using C++20.

This repository is a solution to the above-described problems:
- Atomic/Lock-free/Concurrent data-structures: stacks, queues, spinlocks;
- Performance tests of different thread-safe data structures implementations;
- ConcurrentPatterns: Multiplex, Barrier, SignalQueue, LightSwitch, ProducerConsumer, Double-checked locking;

I'm not claiming that implementations here are the most efficient ones since they are provided for educational purposes.

TODO:
- Add simulation for concurrency patterns;
- Add solutions to common concurrency problems;
- Add hazard pointers implementation;
- Add more implmentations of the concurrent data structures;

Used materials:
- [C++ Concurrency in Action by Anthony Williams](https://www.bogotobogo.com/cplusplus/files/CplusplusConcurrencyInAction_PracticalMultithreading.pdf);
- [The Little Book of Semaphores by Allen B. Downey](https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf);
- https://www.cs.rochester.edu/research/synchronization/pseudocode/queues.html;
- https://rigtorp.se/spinlock/;
- [Concurrency pattern wiki](https://en.wikipedia.org/wiki/Concurrency_pattern);