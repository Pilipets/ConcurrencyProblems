#pragma once

#include <atomic>
#include <thread>

namespace concurrent::ds::locks {

	class TtasLock {
        std::atomic<bool> flag{ false };

        void lock() {
            for (;;) {
                if (!flag.exchange(true, std::memory_order_acquire)) {
                    return;
                }

                while (flag.load(std::memory_order_relaxed)) {
                    std::this_thread::yield();
                }
            }
        }

        bool try_lock() {
            return !flag.load(std::memory_order_relaxed) && !flag.exchange(true, std::memory_order_acquire);
        }

        void unlock() {
            flag.store(false, std::memory_order_release);
        }
	};

}