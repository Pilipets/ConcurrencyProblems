#include "Barrier.h"
#include "CyclicBarrier.h"
#include "LightSwitch.h"
#include "Multiplex.h"
#include "ProducerConsumer.h"
#include "FiniteProducerConsumer.h"
#include "SignalQueue.h"
#include "NoStarveMutex.h"

#include <latch>
#include <barrier>

int main() {
	using namespace concurrent::patterns;

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

	const int buffer_size = 30;
	FiniteProducerConsumer<int> pc3(buffer_size);
	FiniteProducerConsumer<int, std::queue<int>, std::condition_variable> pc4(buffer_size);

	SignalQueue q1;
	ExclusiveSignalQueue q2;

	NoStarveMutex mx;

	return 0;
}