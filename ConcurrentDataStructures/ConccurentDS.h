#pragma once

#include "Queue.h"
#include "Stack.h"
#include "SpinLock.h"

// This is the most reliable options
namespace concurrent::ds {

	template <class T>
	using stack = stacks::ConcurrentStack<T>;

	using spinlock = locks::SpinLock;

	template <class T>
	using queue = queues::ConcurrentQueue<T>;

}