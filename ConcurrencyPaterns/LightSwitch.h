#pragma once

#include <mutex>
#include <functional>

namespace concurrent::primitives {

	// The first thread into a section locks a semaphore(or queues) and the last one out unlocks it.
	class LightSwitch {
		std::mutex mx;
		std::function<void()> lock_cb, unlock_cb;
		int cnt = 0;

		LightSwitch(std::function<void()> lock_cb, std::function<void()> unlock_cb):
			lock_cb(std::move(lock_cb)), unlock_cb(std::move(unlock_cb)) {
		}

	public:
		void lock() {
			std::lock_guard<std::mutex> lk(mx);
			if (cnt++ == 0) lock_cb();
		}

		void unlock() {
			std::lock_guard<std::mutex> lk(mx);
			if (--cnt == 0) unlock_cb();
		}
	};
}
