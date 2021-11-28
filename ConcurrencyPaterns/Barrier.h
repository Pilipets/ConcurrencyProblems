#pragma once

// #include <barrier>
#include <semaphore>

// You are free to use std::barrier, std::latch, however
// it can be achieved using semaphores.

namespace concurrent::primitives {

	template <class Semaphore = std::binary_semaphore>
	class Barrier {
		Semaphore barrier;
		std::atomic_int cnt;

	public:
		Barrier(int n) : barrier(0), cnt(n) {}
		void wait() {
			if (cnt.fetch_sub(1, std::memory_order_acq_rel) == 1)
				barrier.release();

			barrier.acquire();
			barrier.release();
		}
	};

	template <>
	class Barrier <std::counting_semaphore<>> {
		std::counting_semaphore<> barrier;
		std::atomic_int cnt;
		int n;

	public:
		Barrier(int n) : barrier(0), cnt(n), n(n) {}
		void wait() {
			if (cnt.fetch_sub(1, std::memory_order_acq_rel) == 1)
				barrier.release(n);

			barrier.acquire();
		}
	};

}