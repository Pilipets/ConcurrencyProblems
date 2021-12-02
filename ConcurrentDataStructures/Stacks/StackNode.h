#pragma once

#include <memory>

namespace concurrent::ds::stacks {

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
	struct SharedNode {
		using NodePtr = std::shared_ptr<SharedNode>;

		T val;
		NodePtr next;

		SharedNode(T val = T(), NodePtr next = nullptr)
			: val(std::move(val)), next(std::move(next)) {
		}
	};
}