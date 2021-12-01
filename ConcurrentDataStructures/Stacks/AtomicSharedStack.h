#pragma once

#include <atomic>
#include <memory>
#include <optional>

namespace concurrent::ds::stacks {

	template <class T>
	class AtomicSharedStack {
		struct Node;
		using NodePtr = std::shared_ptr<Node>;
		struct Node {
			T val;
			NodePtr next;
			Node(T val) : val(std::move(val)) {}
		};

		std::atomic<NodePtr> head;
		AtomicSharedStack(AtomicSharedStack&) = delete;
		AtomicSharedStack& operator=(AtomicSharedStack&) = delete;

	public:
		AtomicSharedStack() = default;
		~AtomicSharedStack() = default;

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

}