Running tests for class concurrent::ds::stacks::ConcurrentStack<int>...
  Test num    Ingest size    Threads num   Correct          Execution time
        #0         600000              6         1                2669.36ms
        #1          30000             30         1                203.919ms
        #2       20000000             20         1                 354409ms
        #3          50000            100         1                336.849ms
        #4         700000             14         1                9123.19ms
        #5             40              4         1                 0.0148ms

Running tests for class concurrent::ds::stacks::ThreadSafeStack<int,class std::mutex>...
  Test num    Ingest size    Threads num   Correct          Execution time
        #0         600000              6         1                681.894ms
        #1          30000             30         1                51.4098ms
        #2       20000000             20         1                75311.9ms
        #3          50000            100         1                169.147ms
        #4         700000             14         1                1730.45ms
        #5             40              4         1                 0.0067ms

Running tests for class concurrent::ds::stacks::ThreadSafeStack<int,class concurrent::ds::locks::SpinLock>...
  Test num    Ingest size    Threads num   Correct          Execution time
        #0         600000              6         1                115.857ms
        #1          30000             30         1                360.135ms
        #2       20000000             20         1                48678.6ms
        #3          50000            100         1                477.383ms
        #4         700000             14         1                187.192ms
        #5             40              4         1                 0.0054ms

Running tests for class concurrent::ds::stacks::unsafe::AtomicStack<int>...
  Test num    Ingest size    Threads num   Correct          Execution time
        #0         600000              6         1                102.617ms
        #1          30000             30         1                 5.8022ms
        #2       20000000             20         1                8271.99ms
        #3          50000            100         1                 8.2857ms
        #4         700000             14         1                138.907ms
        #5             40              4         1                 0.0039ms

Running tests for class concurrent::ds::queues::ConcurrentQueue<int>...
  Test num    Ingest size    Threads num   Correct          Execution time
        #0         600000              6         1                2751.14ms
        #1          30000             30         1                204.946ms
        #2       20000000             20         1                 415915ms
        #3          50000            100         1                476.904ms
        #4         700000             14         1                9939.62ms
        #5             40              4         1                  0.028ms

Running tests for class concurrent::ds::queues::TwoLockQueue<int,class std::mutex>...
  Test num    Ingest size    Threads num   Correct          Execution time
        #0         600000              6         1                243.321ms
        #1          30000             30         1                 16.032ms
        #2       20000000             20         1                36186.4ms
        #3          50000            100         1                916.529ms
        #4         700000             14         1                284.284ms
        #5             40              4         1                 0.0071ms

Running tests for class concurrent::ds::queues::TwoLockQueue<int,class concurrent::ds::locks::SpinLock>...
  Test num    Ingest size    Threads num   Correct          Execution time
        #0         600000              6         1                111.328ms
        #1          30000             30         1                369.656ms
        #2       20000000             20         1                24411.8ms
        #3          50000            100         1                12.3277ms
        #4         700000             14         1                252.908ms
        #5             40              4         1                 0.0077ms

Running tests for class concurrent::ds::queues::ThreadSafeQueue<int,class std::mutex>...
  Test num    Ingest size    Threads num   Correct          Execution time
        #0         600000              6         1                655.488ms
        #1          30000             30         1                46.9246ms
        #2       20000000             20         1                72764.3ms
        #3          50000            100         1                115.991ms
        #4         700000             14         1                   1841ms
        #5             40              4         1                 0.0086ms

Running tests for class concurrent::ds::queues::ThreadSafeQueue<int,class concurrent::ds::locks::SpinLock>...
  Test num    Ingest size    Threads num   Correct          Execution time
        #0         600000              6         1                110.382ms
        #1          30000             30         1                 9.4398ms
        #2       20000000             20         1                55169.9ms
        #3          50000            100         1                425.191ms
        #4         700000             14         1                401.524ms
        #5             40              4         1                 0.0044ms

Running tests for class concurrent::ds::queues::unsafe::AtomicQueue<int>...
  Test num    Ingest size    Threads num   Correct          Execution time
        #0         600000              6         1                102.823ms
        #1          30000             30         1                  5.567ms
        #2       20000000             20         1                8571.15ms
        #3          50000            100         1                 8.8458ms
        #4         700000             14         1                167.309ms
        #5             40              4         1                  0.004ms

Running tests for class boost::lockfree::stack<int>...
  Test num    Ingest size    Threads num   Correct          Execution time
        #0         600000              6         1                121.871ms
        #1          30000             30         1                 6.4804ms
        #2       20000000             20         1                9536.47ms
        #3          50000            100         1                14.4501ms
        #4         700000             14         1                209.821ms
        #5             40              4         1                 0.0058ms

Running tests for class boost::lockfree::queue<int>...
  Test num    Ingest size    Threads num   Correct          Execution time
        #0         600000              6         1                168.507ms
        #1          30000             30         1                 8.9398ms
        #2       20000000             20         1                12735.5ms
        #3          50000            100         1                15.4827ms
        #4         700000             14         1                306.992ms
        #5             40              4         1                 0.0058ms