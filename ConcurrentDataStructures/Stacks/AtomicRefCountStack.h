#pragma once

#include <atomic>
#include <optional>

namespace concurrent::ds::stacks {

	template <class T>
	class AtomicRefCountStack {
		struct Node;

		struct CountedNode {
			int external_cnt = 0;
			Node* ptr = nullptr;
		};

		struct Node {
			T val;
			std::atomic<int> internal_cnt;
			CountedNode next;

			Node(T val) : val(std::move(val)), internal_cnt(0) {}
		};

		std::atomic<CountedNode> head;
		AtomicRefCountStack(AtomicRefCountStack&) = delete;
		AtomicRefCountStack& operator=(AtomicRefCountStack&) = delete;

		void increase_head_count(CountedNode& old_counter) {
			CountedNode new_counter;
			do
			{
				new_counter = old_counter;
				++new_counter.external_cnt;
			} while (!head.compare_exchange_strong(old_counter, new_counter));
			old_counter.external_cnt = new_counter.external_cnt;
		}

	public:
		AtomicRefCountStack() = default;
		~AtomicRefCountStack() { while (pop()); }

		void push(T val) {
			CountedNode new_node{ 1, new Node(std::move(val)) };
			new_node.ptr->next = head.load();

			while (!head.compare_exchange_weak(new_node.ptr->next, new_node));
		}

		std::optional<T> pop() {
			CountedNode old_head = head.load();

			while (true) {
				increase_head_count(old_head);
				Node* ptr = old_head.ptr;

				if (!ptr) {
					return std::nullopt;
				}
				else if (head.compare_exchange_strong(old_head, ptr->next)) {
					T res = std::move(ptr->val);

					int cnt = old_head.external_cnt - 2;
					if (ptr->internal_cnt.fetch_add(cnt) == -cnt)
						delete ptr;

					return res;
				}
				else if (ptr->internal_cnt.fetch_sub(1) == 1) {
					delete ptr;
				}
			}

		}

		bool empty() const { return head.load().ptr == nullptr; }
	};

}