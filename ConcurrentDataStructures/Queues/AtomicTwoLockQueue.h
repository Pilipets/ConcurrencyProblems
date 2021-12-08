#pragma once

#include <queue>
#include <mutex>
#include <optional>
#include "QeueuNode.h"

namespace concurrent::ds::queues {

	template <class T, class LockType = std::mutex>
	class AtomicTwoLockQueue {
		using Node = AtomicRawNode<T>;
		using NodePtr = Node::NodePtr;

		mutable LockType head_mx, tail_mx;
		NodePtr head, tail;

		AtomicTwoLockQueue(AtomicTwoLockQueue&) = delete;
		AtomicTwoLockQueue& operator=(AtomicTwoLockQueue&) = delete;

	public:
		AtomicTwoLockQueue() : head(new Node()), tail(head) {}
		~AtomicTwoLockQueue() {
			while (pop());
			delete head;
		}

		void push(T val) {
			NodePtr node = new Node(std::move(val));

			std::scoped_lock<LockType> lk(tail_mx);
			tail->next.store(node);
			tail = node;
		}

		std::optional<T> pop() {
			NodePtr node;
			std::optional<T> res;

			{
				std::scoped_lock<LockType> lk(head_mx);
				node = head;
				NodePtr new_head = node->next;
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