#pragma once

#include "Queue.h"
#include "Stack.h"
#include "SpinLock.h"

// This is the most reliable options
namespace concurrent_ds {

	template <class T>
	using stack = stacks::ConcurrentSharedStack<T>;

	using spinlock = locks::SpinLock;

	template <class T>
	using queue = queues::ConcurrentRawQueue<T>;

}