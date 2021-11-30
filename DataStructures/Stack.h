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

		const T& top() const {
			return head.load()->val;
		}

		T& top() {
			return head.load()->val;
		}

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

		void pop() {
			std::scoped_lock<LockType> lk(mx);
			s.pop();
		}
	};

	/*
	namespace unsafe {

		// https://lumian2015.github.io/lockFreeProgramming/
		// We can't safely delete nodes in the below implementation.
		template <class T>
		class AtomicStack {
			struct Node {
				T val;
				Node* next;
			};

			struct TaggedNode {
				uint16_t tag;
				Node* head;
			};
			std::atomic<TaggedNode> tagged_head{};

			AtomicStack(AtomicStack&) = delete;
			AtomicStack& operator=(AtomicStack&) = delete;

		public:
			AtomicStack() = default;
			~AtomicStack() { while (pop()); }

			void push(T val) {
				TaggedNode next, orig = tagged_head.load();

				Node* node = new Node();
				node->val = std::move(val);
				do {
					node->next = orig.head;
					next.head = node;
					next.tag = orig.tag + 1;
				} while (!tagged_head.compare_exchange_weak(orig, next));
			}

			std::optional<T> pop() {
				TaggedNode next, orig = tagged_head.load();
				do {
					if (orig.head == nullptr) return std::nullopt;
					next.head = orig.head->next; // head can be deleted here, so it's not safe to delete
					next.tag = orig.tag + 1;
				} while (!tagged_head.compare_exchange_weak(orig, next));

				T val = std::move(orig.head->val);
				delete orig.head; // we can't delete nodes here
				return val;
			}
		};

	}
	*/

}