#pragma once

#include <queue>
#include <mutex>
#include <optional>

namespace concurrent::ds::queues {

	template <class T, class LockType = std::mutex>
	class LockBasedQueue {
		mutable LockType mx;
		std::queue<T> q;

		LockBasedQueue(LockBasedQueue&) = delete;
		LockBasedQueue& operator=(LockBasedQueue&) = delete;

	public:
		LockBasedQueue() = default;
		~LockBasedQueue() = default;

		void push(T val) {
			std::scoped_lock<LockType> lk(mx);
			q.push(std::move(val));
		}

		std::optional<T> pop() {
			std::scoped_lock<LockType> lk(mx);
			if (q.empty()) return std::nullopt;

			T ret = std::move(q.front());
			q.pop();
			return ret;
		}

		bool empty() const {
			std::scoped_lock<LockType> lk(mx);
			return q.empty();
		}
	};

}