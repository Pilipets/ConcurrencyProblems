#pragma once

#include <queue>
#include <mutex>
#include <optional>

namespace concurrent::ds::queues {

	template <class T, class LockType = std::mutex>
	class TwoLockQueue {
		struct Node {
			T val;
			Node* next = nullptr;

			Node(T val = T()) : val(std::move(val)) {}
		};

		mutable LockType head_mx, tail_mx;
		Node* head, * tail;

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
				Node* new_head = node->next;
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

}