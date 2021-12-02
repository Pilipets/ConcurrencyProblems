#pragma once

#include "Queue.h"
#include "Stack.h"
#include "Lock.h"

// This is the most reliable options
namespace concurrent::ds {

	template <class T>
	using stack = stacks::AtomicReclaimStack<T>;

	using spinlock = locks::TtasLock;

	template <class T>
	using queue = queues::AtomicSharedQueue<T>;

}