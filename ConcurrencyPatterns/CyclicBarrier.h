#pragma once

#include <semaphore>
#include <mutex>

namespace concurrent::primitives {

	// Two-phase barrier
	template <class Semaphore = std::binary_semaphore>
	class CyclicBarrier {
		Semaphore enterTurnstile, exitTurnstile;
		std::mutex mx;
		int cnt, n;

		void enter_wait() {
			{
				std::scoped_lock<std::mutex> lk(mx);
				if (++cnt == n) {
					exitTurnstile.acquire();
					enterTurnstile.release();
				}
			}

			enterTurnstile.acquire();
			enterTurnstile.release();
		}

		void exit_wait() {
			{
				std::scoped_lock<std::mutex> lk(mx);
				if (--cnt == 0) {
					enterTurnstile.acquire();
					exitTurnstile.release();
				}
			}

			exitTurnstile.acquire();
			exitTurnstile.release();
		}

	public:
		CyclicBarrier(int n) : enterTurnstile(0), exitTurnstile(1), cnt(0), n(n) {}

		void wait() {
			enter_wait();
			// Critical point
			exit_wait();
		}
	};

	template<>
	class CyclicBarrier <std::counting_semaphore<>> {
		std::counting_semaphore<> enterTurnstile, exitTurnstile;
		std::mutex mx;
		int cnt, n;

		void enter_wait() {
			{
				std::scoped_lock<std::mutex> lk(mx);
				if (++cnt == n) enterTurnstile.release(n);
			}
			enterTurnstile.acquire();
		}

		void exit_wait() {
			{
				std::scoped_lock<std::mutex> lk(mx);
				if (--cnt == 0) exitTurnstile.release(n);
			}
			exitTurnstile.acquire();
		}

	public:
		CyclicBarrier(int n) : enterTurnstile(0), exitTurnstile(1), cnt(0), n(n) {}

		void wait() {
			enter_wait();
			// Critical point
			exit_wait();
		}
	};
}