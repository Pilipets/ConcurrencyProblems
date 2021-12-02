#pragma once

#include <atomic>
#include <memory>

namespace concurrent::ds::queues {

	template <class T>
	struct RawNode {
		using NodePtr = RawNode*;

		T val;
		NodePtr next;

		RawNode(T val = T(), NodePtr next = nullptr)
			: val(std::move(val)), next(next) {
		}
	};

	template <class T>
	struct AtomicRawNode {
		using NodePtr = AtomicRawNode*;

		T val;
		std::atomic<NodePtr> next;

		AtomicRawNode(T val = T(), NodePtr next = nullptr)
			: val(std::move(val)), next(next) {
		}
	};

	template <class T>
	struct AtomicSharedNode {
		using NodePtr = std::shared_ptr<AtomicSharedNode>;

		T val;
		std::atomic<NodePtr> next;

		AtomicSharedNode(T val = T(), NodePtr next = nullptr)
			: val(std::move(val)), next(std::move(next)) {
		}
	};
}