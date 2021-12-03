#pragma once

#include <mutex>
#include <semaphore>

namespace concurrent::patterns {

	// Morris's solution - assuming the number of threads is finite.
	class NoStarveMutex {
		std::binary_semaphore mx;
		std::counting_semaphore<> t1, t2;
		int room1, room2;

	public:
		NoStarveMutex() : mx(1), t1(1), t2(0), room1(0), room2(0) {}

		void lock() {
			mx.acquire();
			room1 += 1;
			mx.release();

			t1.acquire();
			room2 += 1;
			mx.acquire();
			room1 -= 1;

			if (room1 == 0) {
				mx.release();
				t2.release();
			}
			else {
				mx.release();
				t1.release();
			}

			t2.acquire();
			room2 -= 1;
		}

		void unlock() {
			if (room2 == 0) {
				t1.release();
			}
			else {
				t2.release();
			}
		}
	};

}