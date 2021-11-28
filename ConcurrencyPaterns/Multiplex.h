#pragma once

#include <semaphore>

namespace concurrent::primitives {

	// Multiplex concurrency patttern:
	// no more than n threads can run in the critical section at the same time.
	template <class Semaphore = std::counting_semaphore<>>
	class Multiplex {

		// mutex is not possible, cause of thread-owning nature of mutex
		Semaphore sem;
	public:
		Multiplex(int n) : sem(n) {}

		void wait() { sem.acquire(); }
		void signal() { sem.release(); }
	};

}