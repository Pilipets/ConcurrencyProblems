#pragma once

#include <atomic>
#include "StackNode.h"

namespace concurrent::ds::stacks::unsafe {

	template <class T>
	class AtomicLeakStack {
		using Node = RawNode<T>;
		using NodePtr = Node::NodePtr;

		std::atomic<NodePtr> head;
		AtomicLeakStack(AtomicLeakStack&) = delete;
		AtomicLeakStack& operator=(AtomicLeakStack&) = delete;

	public:
		AtomicLeakStack() = default;
		~AtomicLeakStack() = default;

		void push(T val) {
			auto p = new Node(std::move(val));
			p->next = head.load();

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