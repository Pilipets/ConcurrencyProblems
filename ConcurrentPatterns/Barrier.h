#pragma once

// #include <barrier>
#include <semaphore>

// You are free to use std::barrier, std::latch, however
// it can be achieved using semaphores.

namespace concurrent::patterns {

	template <class Semaphore = std::counting_semaphore<>>
	class Barrier {
		Semaphore barrier;
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

	template <>
	class Barrier<std::binary_semaphore> {
		std::binary_semaphore barrier;
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

}