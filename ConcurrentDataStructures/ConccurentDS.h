#pragma once

#include "Queue.h"
#include "Stack.h"
#include "Lock.h"

// Here are the most efficient options
namespace concurrent::ds {

	template <class T>
	using stack = stacks::AtomicReclaimStack<T>;

	using spinlock = locks::TtasLock;

	template <class T>
	using queue = queues::AtomicReclaimQueue<T>;

}