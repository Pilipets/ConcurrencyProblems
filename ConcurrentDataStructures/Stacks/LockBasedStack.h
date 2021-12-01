#pragma once

#include <mutex>
#include <optional>
#include <stack>

namespace concurrent::ds::stacks {

	template <class T, class LockType = std::mutex>
	class LockBasedStack {
		mutable LockType mx;
		std::stack<T> s;

		LockBasedStack(LockBasedStack&) = delete;
		LockBasedStack& operator=(LockBasedStack&) = delete;

	public:
		LockBasedStack() = default;
		~LockBasedStack() = default;

		void push(T val) {
			std::scoped_lock<LockType> lk(mx);
			s.push(std::move(std::move(val)));
		}

		std::optional<T> pop() {
			std::scoped_lock<LockType> lk(mx);
			if (s.empty()) return std::nullopt;

			int ret = s.top();
			s.pop();
			return ret;
		}

		bool empty() const {
			std::scoped_lock<LockType> lk(mx);
			return s.empty();
		}
	};

}