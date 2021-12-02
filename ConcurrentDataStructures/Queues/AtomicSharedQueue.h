#pragma once

#include <optional>
#include "QeueuNode.h"

namespace concurrent::ds::queues {

	template <class T>
	class AtomicSharedQueue {
		using Node = AtomicSharedNode<T>;
		using NodePtr = Node::NodePtr;

		AtomicSharedQueue(AtomicSharedQueue&) = delete;
		AtomicSharedQueue& operator=(AtomicSharedQueue&) = delete;

		std::atomic<NodePtr> head, tail;

	public:
		AtomicSharedQueue() : head(std::make_shared<Node>()), tail(head.load()) {}
		~AtomicSharedQueue() = default;

		void push(T val) {
			NodePtr new_node = std::make_shared<Node>(std::move(val));

			while (true) {
				NodePtr old_tail = tail.load(), dummy{};

				if (old_tail->next.compare_exchange_weak(dummy, new_node)) {
					tail.compare_exchange_weak(old_tail, new_node);
					return;
				}
				else {
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
						tail.compare_exchange_weak(old_tail, next_head);
					}
				}
				else {
					if (head.compare_exchange_weak(old_head, next_head))
						return next_head->val;
				}
			}
		}

		bool empty() const { return head.load()->next.load() == nullptr; }
	};

}