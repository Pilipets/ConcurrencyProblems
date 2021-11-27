#pragma once

#include <atomic>
#include <memory>
#include <queue>
#include <mutex>
#include <optional>

namespace concurrent_ds::queues {

	// http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.53.8674&rep=rep1&type=pdf
	// https://neerc.ifmo.ru/wiki/index.php?title=%D0%9E%D1%87%D0%B5%D1%80%D0%B5%D0%B4%D1%8C_%D0%9C%D0%B0%D0%B9%D0%BA%D0%BB%D0%B0_%D0%B8_%D0%A1%D0%BA%D0%BE%D1%82%D1%82%D0%B0
	template <class T>
	class ConcurrentRawQueue {
		struct Node {
			T val;
			std::atomic<Node*> next;

			Node(T val = T()) : val(std::move(val)), next(nullptr) {}
		};

		ConcurrentRawQueue(ConcurrentRawQueue&) = delete;
		ConcurrentRawQueue& operator=(ConcurrentRawQueue&) = delete;

		std::atomic<Node*> head, tail;

	public:
		ConcurrentRawQueue() : head(new Node()), tail(head.load()) {}
		~ConcurrentRawQueue() { while (pop()); }

		void push(T val) {
			std::atomic<Node*> new_node = new Node(std::move(val));

			while (true) {
				Node *old_tail = tail.load(), *dummy = nullptr;

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
				Node* old_head = head.load(), *old_tail = tail.load();
				Node *next_head = old_head->next.load();

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
	};

	// https://www.cs.rochester.edu/research/synchronization/pseudocode/queues.html
	template <class T, class LockType = std::mutex>
	class TwoLockQueue {
		struct Node {
			T val;
			Node* next = nullptr;
		};

		mutable LockType head_mx, tail_mx;
		Node *head, *tail;

		TwoLockQueue(TwoLockQueue&) = delete;
		TwoLockQueue& operator=(TwoLockQueue&) = delete;

	public:
		TwoLockQueue() : head(new Node()), tail(head) {}
		~TwoLockQueue() { delete head; }

		void push(T val) {
			Node* node = new Node();
			node->val = std::move(val);

			std::scoped_lock<LockType> lk(tail_mx);
			tail->next = node;
			tail = node;
		}

		std::optional<T> pop() {
			Node* node;
			std::optional<T> res;

			{
				std::scoped_lock<LockType> lk(head_mx);
				*node = head;
				Node *new_head = node->next;
				if (new_head == nullptr) return res;

				*res = std::move(new_head->val);
				head = new_head;
			}

			delete node;
			return res;
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

		void push(T t) {
			std::scoped_lock<LockType> lk(mx);
			q.push(std::move(t));
		}

		void pop() {
			std::scoped_lock<LockType> lk(mx);
			q.pop();
		}
	};
}