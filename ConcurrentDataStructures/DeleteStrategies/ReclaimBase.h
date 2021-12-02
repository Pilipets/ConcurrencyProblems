#pragma once

#include <atomic>

namespace concurrent::ds::memory {

	template <class Node>
	class ReclaimBase {
		using NodePtr = Node::NodePtr;
	protected:
		std::atomic<unsigned> threads_in_pop;
		std::atomic<NodePtr> delete_head;

		ReclaimBase(ReclaimBase&) = delete;
		ReclaimBase& operator=(ReclaimBase&) = delete;

		ReclaimBase() : threads_in_pop(0), delete_head(nullptr) {}

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
	};

}