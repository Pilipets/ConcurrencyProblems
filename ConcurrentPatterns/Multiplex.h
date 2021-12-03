#pragma once

#include <semaphore>

namespace concurrent::patterns {

	// Multiplex concurrency patttern:
	// no more than n threads can run in the critical section at the same time.
	template <class Semaphore = std::counting_semaphore<>>
	class Multiplex {
		// mutex is not possible because of thread-owning nature of the mutex.
		Semaphore sem;

	public:
		Multiplex(int n) : sem(n) {}

		void wait() { sem.acquire(); }
		void signal() { sem.release(); }
	};

}