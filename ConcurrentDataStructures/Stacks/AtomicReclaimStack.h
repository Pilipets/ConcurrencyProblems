#pragma once

#include <atomic>
#include "Common.h"

namespace concurrent::ds::stacks {

	template <class T>
	class AtomicReclaimStack {
		using Node = RawNode<T>;
		using NodePtr = Node::NodePtr;

		std::atomic<unsigned> threads_in_pop;
		std::atomic<NodePtr> delete_head;

		std::atomic<NodePtr> head;
		AtomicReclaimStack(AtomicReclaimStack&) = delete;
		AtomicReclaimStack& operator=(AtomicReclaimStack&) = delete;

		void chain_pending_nodes(NodePtr first, NodePtr last) {
			last->next = delete_head;
			while (!delete_head.compare_exchange_weak(last->next, first));
		}

		void chain_pending_nodes(NodePtr head) {
			NodePtr last = head;
			while (last->next) last = last->next;
			chain_pending_nodes(head, last);
		}

		static void delete_nodes(NodePtr delete_head) {
			while (delete_head) {
				NodePtr next = delete_head->next;
				delete delete_head;
				delete_head = next;
			}
		}

		void try_reclaim(NodePtr old_head) {
			if (threads_in_pop == 1) {
				NodePtr nodes_to_delete = delete_head.exchange(nullptr);

				if (!--threads_in_pop) delete_nodes(nodes_to_delete);
				else if (nodes_to_delete) chain_pending_nodes(nodes_to_delete);
				delete old_head;
			}
			else {
				chain_pending_nodes(old_head, old_head);
				--threads_in_pop;
			}
		}

	public:
		AtomicReclaimStack() : threads_in_pop(0), delete_head(nullptr), head(nullptr) {};
		~AtomicReclaimStack() { while (pop()); }

		void push(T val) {
			auto new_head = new Node(std::move(val));
			new_head->next = head;

			while (!head.compare_exchange_weak(new_head->next, new_head));
		}

		std::optional<T> pop() {
			++threads_in_pop;

			auto old_head = head.load();
			while (old_head && !head.compare_exchange_weak(old_head, old_head->next));

			if (!old_head) return std::nullopt;

			T res = std::move(old_head->val);
			try_reclaim(old_head);
			return res;
		}

		bool empty() const { return head.load() == nullptr; }
	};

}