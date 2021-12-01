#pragma once

#include <atomic>
#include <memory>
#include <stack>
#include <mutex>
#include <optional>


namespace concurrent::ds::stacks {

	template <class T>
	class AtomicReclaimStack {
		struct Node;
		using NodePtr = Node*;
		struct Node {
			T val;
			NodePtr next;
			Node(T val) : val(std::move(val)) {}
		};

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
		AtomicReclaimStack() : threads_in_pop(0), delete_head(nullptr), head(nullptr) {} ;
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

			std::optional<T> res = std::move(old_head->val);
			try_reclaim(old_head);
			return res;
		}

		bool empty() const { return head.load() == nullptr; }
	};

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

	template <class T, class LockType = std::mutex>
	class ThreadSafeStack {
		mutable LockType mx;
		std::stack<T> s;

		ThreadSafeStack(ThreadSafeStack&) = delete;
		ThreadSafeStack& operator=(ThreadSafeStack&) = delete;

	public:
		ThreadSafeStack() = default;
		~ThreadSafeStack() = default;

		void push(T val) {
			std::scoped_lock<LockType> lk(mx);
			s.push(std::move(std::move(val)));
		}

		std::optional<T> pop() {
			std::scoped_lock<LockType> lk(mx);
			if (s.empty()) return std::nullopt;

			int ret = s.top();
			s.pop();
			return ret;
		}

		bool empty() const {
			std::scoped_lock<LockType> lk(mx);
			return s.empty();
		}
	};

	namespace unsafe {

		template <class T>
		class AtomicLeakStack {
			struct Node;
			using NodePtr = Node*;
			struct Node {
				T val;
				NodePtr next;
			};

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

}