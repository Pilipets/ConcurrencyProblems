#include "ConccurentDS.h"

#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <typeinfo>
#include <typeindex>
#include <iomanip>
#include <type_traits>

namespace concurrent::ds::testing {

	template <class Func, class... Args>
	auto timeFuncInvocation(Func&& func, Args&&... args) {
		auto start = std::chrono::high_resolution_clock::now();
		std::invoke_result_t<Func, Args...> res = std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
		auto stop = std::chrono::high_resolution_clock::now();
		return std::make_pair(std::move(res), stop - start);
	};

	template <class Duration, class Func, class... Args>
	auto successTimeFuncInvocation(Func&& func, Args&&... args) {
		while (true) {
			auto res = timeFuncInvocation(std::forward<Func>(func), std::forward<Args>(args)...);
			if (res.first) return Duration(res.second);
		}
		return Duration{};
	}

	template<class Duration, class Func, class... Args>
	void executeAddTime(Duration& duration, std::mutex& mx, size_t n, Func&& func, Args&&... args) {
		for (size_t i = 0; i < n; ++i) {
			auto dur = successTimeFuncInvocation<Duration>(std::forward<Func>(func), std::forward<Args>(args)...);

			std::scoped_lock<std::mutex> lk(mx);
			duration += dur;
		}
	}

	template <class Duration, class Container, class Produce, class Consume, class Pred, class DataIngestor, class... Args>
	std::pair<bool, Duration> testContainerProducerConsumer(Produce&& produce, Consume&& consume,
		Pred&& postCondition, DataIngestor get_next_data, size_t produce_n, size_t consume_n, size_t threads_num,
		Args&&... args) {

		Container c(std::forward<Args>(args)...);

		std::vector<std::thread> threads(2 * threads_num);
		std::mutex mx;
		Duration duration{};
		for (size_t i = 0; i < threads_num; ++i) {
			threads[i] = std::thread([&] {
				for (size_t j = 0; j < produce_n; ++j) {
					auto dur = successTimeFuncInvocation<Duration>(std::forward<Produce>(produce), &c, get_next_data());

					std::scoped_lock<std::mutex> lk(mx);
					duration += dur;
				}
				//executeAddTime(duration, mx, produce_n, std::forward<Produce>(produce), &c, get_next_data());
			});

			threads[i + threads_num] = std::thread([&] {
				for (size_t j = 0; j < consume_n; ++j) {
					auto dur = successTimeFuncInvocation<Duration>(std::forward<Consume>(consume), &c);

					std::scoped_lock<std::mutex> lk(mx);
					duration += dur;
				}
				//executeAddTime(duration, mx, consume_n, std::forward<Consume>(consume), &c);
			});
		}

		for (size_t i = 0; i < threads.size(); ++i)
			threads[i].join();

		return { postCondition(&c), duration };
	}

	template <class Duration, class Container, class Produce, class Consume, class Pred, class DataIngestor, class... Args>
	std::vector<std::pair<bool, Duration>> getTestResults(Produce&& produce, Consume&& consume,
		Pred&& pred, DataIngestor&& get_next_data, std::vector<std::array<int, 3>> input_data,
		bool verbose, Args&&... args) {

		std::vector<std::pair<bool, Duration>> res(input_data.size());

		if (verbose) {
			std::cout << "Running tests for " << typeid(Container).name() << "...\n";
		}
		for (size_t i = 0; i < res.size(); ++i) {
			res[i] = testContainerProducerConsumer<Duration, Container>(
				std::forward<Produce>(produce),
				std::forward<Consume>(consume),
				std::forward<Pred>(pred),
				std::forward<DataIngestor>(get_next_data),
				input_data[i][0],
				input_data[i][1],
				input_data[i][2],
				std::forward<Args>(args)...
			);
		}

		if (verbose) {
			std::cout << std::setw(10) << "Test num" << std::setw(15) << "Ingest size";
			std::cout << std::setw(15) << "Threads num";
			std::cout << std::setw(10) << "Correct" << std::setw(25) << "Execution time\n";
			for (size_t i = 0; i < res.size(); ++i) {
				std::cout << std::setw(10) << "#" + std::to_string(i);
				std::cout << std::setw(15) << std::to_string((input_data[i][0] + input_data[i][1]) * input_data[i][2]);
				std::cout << std::setw(15) << std::to_string(input_data[i][2] * 2);
				std::cout << std::setw(10) << res[i].first ? "True" : "False";
				std::cout << std::setw(25) << res[i].second;
				std::cout << "\n";
			}
			std::cout << "\n";
		}

		return res;
	}

	template<typename...>
	struct TypeList {};

	template<typename T>
	using Type = TypeList<T>;

	template<typename F, typename... Ts>
	void for_each(TypeList<Ts...>, F f) {
		(..., f(std::type_identity<Ts>{}));
	}

}

#include <boost/lockfree/stack.hpp>
#include <boost/lockfree/queue.hpp>

void testDataStructures() {
	using namespace concurrent::ds;
	using Duration = std::chrono::duration<double, std::ratio<1, 1000>>;

	std::vector<std::array<int, 3>> producers_consumers_setup = {
		{100000, 100000, 3},
		{1000, 1000, 15},
		{1000000, 1000000, 10},
		{5000, 5000, 50},
		{50000, 50000, 7},
		{10, 10, 2}
	};

	testing::for_each(testing::TypeList<
		stacks::AtomicSharedStack<int>,
		stacks::AtomicReclaimStack<int>,
		stacks::ThreadSafeStack<int, std::mutex>,
		stacks::ThreadSafeStack<int, locks::SpinLock>,
		stacks::unsafe::AtomicLeakStack<int>/*,
		queues::ConcurrentQueue<int>,
		queues::TwoLockQueue<int>,
		queues::TwoLockQueue<int, locks::SpinLock>,
		queues::ThreadSafeQueue<int>,
		queues::ThreadSafeQueue<int, locks::SpinLock>,
		queues::unsafe::AtomicQueue<int>*/
		>
		{}
		, [&](auto arg) {

		using Container = decltype(arg)::type;

		testing::getTestResults<Duration, Container>(
			[&](Container* c, int val) { c->push(val); return true; },
			[&](Container* c) { return c->pop(); },
			[&](Container* c) { return c->empty(); },
			[] { return rand(); },
			producers_consumers_setup,
			true
		);
	});

	testing::for_each(testing::TypeList<
		boost::lockfree::stack<int>/*,
		boost::lockfree::queue<int>*/
	>
		{}
	, [&](auto arg) {

		using Container = decltype(arg)::type;

		testing::getTestResults<Duration, Container>(
			[&](Container* c, int val) { c->push(val); return true; },
			[&](Container* c) { int val;  return c->pop(val); },
			[&](Container* c) { return c->empty(); },
			[] { return rand(); },
			producers_consumers_setup,
			true,
			0
		);
	});

}

int main() {
	testDataStructures();
	return 0;
}