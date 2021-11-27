#pragma once

#include <atomic>
#include <memory>
#include <stack>
#include <mutex>
#include <optional>

namespace concurrent_ds::stacks {

	// Because of using std::shared_ptr, the ABA problem is gone - however,
	// this is not lock-free because of spinlock underneath.
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4162.pdf
	template <class T>
	class ConcurrentSharedStack {
		struct Node {
			T val;
			std::shared_ptr<Node> next;
		};

		std::atomic<std::shared_ptr<Node>> head; // std::shared_ptr<Node> head;
		ConcurrentSharedStack(ConcurrentSharedStack&) = delete;
		ConcurrentSharedStack& operator=(ConcurrentSharedStack&) = delete;

	public:
		ConcurrentSharedStack() = default;
		~ConcurrentSharedStack() = default;

		class Reference {
			std::shared_ptr<Node> p;
		public:
			Reference(std::shared_ptr<Node> p) : p(p) {}
			T& operator* () { return p->val; }
			T* operator->() { return &p->val; }
		};

		Reference find(const T& val) const {
			auto p = head.load(); // std::atomic_load(&head);
			while (p && p->val != val) p = p->next;
			return Reference(std::move(p));
		}

		Reference top() const {
			return Reference(head); // std::atomic_load(&head)
		}

		void push(T val) {
			auto p = std::make_shared<Node>();
			p->val = std::move(val);
			p->next = head; // std::atomic_load(&head);

			while (!head.compare_exchange_weak(p->next, p)); // std::atomic_compare_exchange_weak(&head, &p->next, p));
		}

		Reference pop() {
			auto p = std::atomic_load(&head);
			while (p && !head.compare_exchange_weak(p, p->next)); // std::atomic_compare_exchange_weak(&head, &p, p->next));
			return Reference(std::move(p));
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

		void push(T t) {
			std::scoped_lock<LockType> lk(mx);
			s.push(std::move(t));
		}

		void pop() {
			std::scoped_lock<LockType> lk(mx);
			s.pop();
		}
	};

	// https://lumian2015.github.io/lockFreeProgramming/
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
				next.head = orig.head->next;
				next.tag = orig.tag + 1;
			} while (!tagged_head.compare_exchange_weak(orig, next));

			T val = std::move(orig.head->val);
			delete orig.head; // is this really safe?
			return val;
		}
	};

}