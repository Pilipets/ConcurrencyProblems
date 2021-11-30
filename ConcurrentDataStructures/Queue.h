#pragma once

#include <atomic>
#include <memory>
#include <queue>
#include <mutex>
#include <optional>

namespace concurrent::ds::queues {

	template <class T>
	class ConcurrentQueue {
		struct Node;
		using NodePtr = std::shared_ptr<Node>;
		struct Node {
			T val;
			std::atomic<NodePtr> next;

			Node(T val = T()) : val(std::move(val)), next{} {}
		};

		ConcurrentQueue(ConcurrentQueue&) = delete;
		ConcurrentQueue& operator=(ConcurrentQueue&) = delete;

		std::atomic<NodePtr> head, tail;

	public:
		ConcurrentQueue() : head(std::make_shared<Node>()), tail(head.load()) {}
		~ConcurrentQueue() = default;

		void push(T val) {
			std::atomic<NodePtr> new_node = std::make_shared<Node>(std::move(val));

			while (true) {
				NodePtr old_tail = tail.load(), dummy{};

				if (old_tail->next.compare_exchange_weak(dummy, new_node)) {
					tail.compare_exchange_weak(old_tail, new_node);
					return;
				}
				else {
					// Help to move tail to ensure lock-free
					tail.compare_exchange_weak(old_tail, old_tail->next.load());
				}
			}
		}

		std::optional<T> pop() {
			while (true) {
				NodePtr old_head = head, old_tail = tail;
				NodePtr next_head = old_head->next;

				if (old_head == old_tail) {
					if (next_head == nullptr) return std::nullopt;
					else {
						// Help to move tail to ensure lock-free
						tail.compare_exchange_weak(old_tail, next_head);
					}
				}

				else {
					if (head.compare_exchange_weak(old_head, next_head))
						return next_head->val;

				}
			}
		}

		bool empty() const {
			return head.load()->next.load() == nullptr;
		}
	};

	template <class T, class LockType = std::mutex>
	class TwoLockQueue {
		struct Node {
			T val;
			Node* next = nullptr;

			Node(T val = T()) : val(std::move(val)) {}
		};

		mutable LockType head_mx, tail_mx;
		Node *head, *tail;

		TwoLockQueue(TwoLockQueue&) = delete;
		TwoLockQueue& operator=(TwoLockQueue&) = delete;

	public:
		TwoLockQueue() : head(new Node()), tail(head) {}
		~TwoLockQueue() {
			while (pop());
			delete head;
		}

		void push(T val) {
			Node* node = new Node(std::move(val));

			std::scoped_lock<LockType> lk(tail_mx);
			tail->next = node;
			tail = node;
		}

		std::optional<T> pop() {
			Node* node;
			std::optional<T> res;

			{
				std::scoped_lock<LockType> lk(head_mx);
				node = head;
				Node *new_head = node->next;
				if (new_head == nullptr) return std::nullopt;

				res = std::move(new_head->val);
				head = new_head;
			}

			
			delete node;
			return res;
		}

		bool empty() const {
			std::scoped_lock<LockType> lk(head_mx);
			return head->next == nullptr;
		}
	};

	template <class T, class LockType = std::mutex>
	class ThreadSafeQueue {
		mutable LockType mx;
		std::queue<T> q;

		ThreadSafeQueue(ThreadSafeQueue&) = delete;
		ThreadSafeQueue& operator=(ThreadSafeQueue&) = delete;

	public:
		ThreadSafeQueue() = default;
		~ThreadSafeQueue() = default;

		T& front() {
			std::scoped_lock<LockType> lk(mx);
			return q.front();
		}

		const T& front() const {
			std::scoped_lock<LockType> lk(mx);
			return q.front();
		}

		void push(T val) {
			std::scoped_lock<LockType> lk(mx);
			q.push(std::move(val));
		}

		std::optional<T> pop() {
			std::scoped_lock<LockType> lk(mx);
			if (q.empty()) return std::nullopt;

			int ret = q.front();
			q.pop();
			return ret;
		}

		bool empty() const {
			std::scoped_lock<LockType> lk(mx);
			return q.empty();
		}
	};

	namespace unsafe {

		template <class T>
		class AtomicQueue {
			struct Node;
			using NodePtr = Node*;
			struct Node {
				T val;
				std::atomic<NodePtr> next;

				Node(T val = T()) : val(std::move(val)), next{} {}
			};

			AtomicQueue(AtomicQueue&) = delete;
			AtomicQueue& operator=(AtomicQueue&) = delete;

			std::atomic<NodePtr> head, tail;

		public:
			AtomicQueue() : head(new Node()), tail(head.load()) {}
			~AtomicQueue() = default;

			void push(T val) {
				std::atomic<NodePtr> new_node = new Node(std::move(val));

				while (true) {
					NodePtr old_tail = tail.load(), dummy{};

					if (old_tail->next.compare_exchange_weak(dummy, new_node)) {
						tail.compare_exchange_weak(old_tail, new_node);
						return;
					}
					else {
						// Help to move tail to ensure lock-free
						tail.compare_exchange_weak(old_tail, old_tail->next.load());
					}
				}
			}

			std::optional<T> pop() {
				while (true) {
					NodePtr old_head = head, old_tail = tail;
					NodePtr next_head = old_head->next;

					if (old_head == old_tail) {
						if (next_head == nullptr) return std::nullopt;
						else {
							// Help to move tail to ensure lock-free
							tail.compare_exchange_weak(old_tail, next_head);
						}
					}

					else {
						if (head.compare_exchange_weak(old_head, next_head))
							return next_head->val;

					}
				}
			}

			bool empty() const {
				return head.load()->next.load() == nullptr;
			}
		};

	}

}