Recently I encountered a problem with finding easy-to-use neat atomic/concurrent data structures and solutions to common concurrency problems using C++20.

This repository is a solution to the above-described problems:
- Atomic/Concurrent data-structures: stack, queue, spinlock;

TODO:
- Add solutions to common concurrency problems mentioned here - https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf;
- Test concurrent data structures;
- Add the proper memory model to speed up;

All the links used:
--------------------------------------------------------------------------------------
- https://livebook.manning.com/book/c-plus-plus-concurrency-in-action/chapter-7/155;
- https://stackoverflow.com/questions/38984153/how-can-i-implement-aba-counter-with-c11-cas/38991835;
- https://moodycamel.com/blog/2014/solving-the-aba-problem-for-lock-free-free-lists;
- http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.53.8674&rep=rep1&type=pdf;
- https://neerc.ifmo.ru/wiki/index.php?title=%D0%9E%D1%87%D0%B5%D1%80%D0%B5%D0%B4%D1%8C_%D0%9C%D0%B0%D0%B9%D0%BA%D0%BB%D0%B0_%D0%B8_%D0%A1%D0%BA%D0%BE%D1%82%D1%82%D0%B0;
- https://www.cs.rochester.edu/research/synchronization/pseudocode/queues.html;

--------------------------------------------------------------------------------------
- https://stackoverflow.com/questions/45772887/real-world-example-where-stdatomiccompare-exchange-used-with-two-memory-orde;
- http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4162.pdf;
- https://lumian2015.github.io/lockFreeProgramming/;
