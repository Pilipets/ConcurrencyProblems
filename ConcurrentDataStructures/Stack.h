#pragma once

#include <atomic>
#include <memory>
#include <stack>
#include <mutex>
#include <optional>


namespace concurrent::ds::stacks {

	template <class T>
	class ConcurrentStack {
		struct Node;
		using NodePtr = std::shared_ptr<Node>;
		struct Node {
			T val;
			NodePtr next;
			Node(T val) : val(std::move(val)) {}
		};

		std::atomic<NodePtr> head;
		ConcurrentStack(ConcurrentStack&) = delete;
		ConcurrentStack& operator=(ConcurrentStack&) = delete;

	public:
		ConcurrentStack() = default;
		~ConcurrentStack() = default;

		const T& top() const { return head.load()->val; }
		T& top() { return head.load()->val; }

		NodePtr find(const T& val) const {
			auto p = head.load();

			while (p && p->val != val) p = p->next;
			return p;
		}

		void push(T val) {
			auto p = std::make_shared<Node>(std::move(val));
			p->next = head;

			while (!head.compare_exchange_weak(p->next, p));
		}

		std::optional<T> pop() {
			auto p = head.load();

			while (p && !head.compare_exchange_weak(p, p->next));
			return p ? std::optional<T>(std::move(p->val)) : std::nullopt;
		}

		bool empty() const { return head.load() == nullptr; }
	};

	template <class T, class LockType = std::mutex>
	class ThreadSafeStack {
		mutable LockType mx;
		std::stack<T> s;

		ThreadSafeStack(ThreadSafeStack&) = delete;
		ThreadSafeStack& operator=(ThreadSafeStack&) = delete;

	public:
		ThreadSafeStack() = default;
		~ThreadSafeStack() = default;

		T& top() {
			std::scoped_lock<LockType> lk(mx);
			return s.top();
		}

		const T& top() const {
			std::scoped_lock<LockType> lk(mx);
			return s.top();
		}

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

	namespace unsafe {

		template <class T>
		class AtomicStack {
			struct Node;
			using NodePtr = Node*;
			struct Node {
				T val;
				NodePtr next;
			};

			std::atomic<NodePtr> head;

			AtomicStack(AtomicStack&) = delete;
			AtomicStack& operator=(AtomicStack&) = delete;

		public:
			AtomicStack() = default;
			~AtomicStack() = default;

			T& top() { return head.load()->val; }
			const T& top() const { return head.load()->val; }

			void push(T val) {
				auto p = new Node(std::move(val));
				p->next = head;

				while (!head.compare_exchange_weak(p->next, p));
			}

			std::optional<T> pop() {
				auto p = head.load();

				while (p && !head.compare_exchange_weak(p, p->next));
				return p ? std::optional<T>(std::move(p->val)) : std::nullopt;
			}

			bool empty() const { return head.load() == nullptr; }
		};

	}

}