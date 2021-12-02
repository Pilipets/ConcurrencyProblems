#pragma once

#include <atomic>
#include "StackNode.h"
#include "../DeleteStrategies/ReclaimBase.h"

namespace concurrent::ds::stacks {

	template <class T>
	class AtomicReclaimStack : memory::ReclaimBase<RawNode<T>> {
		using Node = RawNode<T>;
		using NodePtr = Node::NodePtr;
		using Base = memory::ReclaimBase<Node>;

		std::atomic<NodePtr> head;
		AtomicReclaimStack(AtomicReclaimStack&) = delete;
		AtomicReclaimStack& operator=(AtomicReclaimStack&) = delete;

	public:
		AtomicReclaimStack() : head(nullptr) {};
		~AtomicReclaimStack() { while (pop()); }

		void push(T val) {
			auto new_head = new Node(std::move(val));
			new_head->next = head;

			while (!head.compare_exchange_weak(new_head->next, new_head));
		}

		std::optional<T> pop() {
			++Base::threads_in_pop;

			auto old_head = head.load();
			while (old_head && !head.compare_exchange_weak(old_head, old_head->next));

			if (!old_head) return std::nullopt;

			T res = std::move(old_head->val);
			Base::try_reclaim(old_head);
			return res;
		}

		bool empty() const { return head.load() == nullptr; }
	};

}