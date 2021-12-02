#include "Barrier.h"
#include "CyclicBarrier.h"
#include "LightSwitch.h"
#include "Multiplex.h"
#include "ProducerConsumer.h"
#include "SignalQueue.h"
#include <latch>
#include <barrier>

int main() {
	using namespace concurrent::primitives;

	const int threads_num = 5;
	Barrier b1(threads_num);
	Barrier<std::binary_semaphore> b2(threads_num);
	std::latch b3(threads_num);

	CyclicBarrier c1(threads_num);
	CyclicBarrier<std::binary_semaphore> c2(threads_num);
	std::barrier c3(threads_num);

	LightSwitch l_switch ([] {}, [] {});

	Multiplex m(threads_num);

	ProducerConsumer<int> pc1;
	ProducerConsumer<int, std::condition_variable> pc2;

	SignalQueue q1;
	ExclusiveSignalQueue q2;

	return 0;
}