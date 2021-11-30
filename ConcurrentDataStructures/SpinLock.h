#pragma once

#include <atomic>

namespace concurrent::ds::locks {

	class SpinLock {
        std::atomic_flag flag = ATOMIC_FLAG_INIT;

    public:
        SpinLock() {}

        void lock() {
            while (flag.test_and_set(std::memory_order_acquire));
        }

        void unlock() {
            flag.clear(std::memory_order_release);
        }
	};

}