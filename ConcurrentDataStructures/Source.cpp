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
#include <random>

using namespace std;
using namespace concurrent::ds;

namespace concurrent::ds::testing {

	template <class Func, class... Args>
	auto timeFuncInvocation(Func&& func, Args&&... args) {
		auto start = chrono::high_resolution_clock::now();
		invoke_result_t<Func, Args...> res = invoke(forward<Func>(func), forward<Args>(args)...);
		auto stop = chrono::high_resolution_clock::now();
		return make_pair(move(res), stop - start);
	};

	template <class Duration, class Func, class... Args>
	auto successTimeFuncInvocation(Func&& func, Args&&... args) {
		while (true) {
			auto res = timeFuncInvocation(forward<Func>(func), forward<Args>(args)...);
			if (res.first) return Duration(res.second);
		}
		return Duration{};
	}

	template<class Duration>
	void sleepAddDuration(size_t i, const vector<size_t>& sleeping_time, Duration &duration, Duration dur, mutex& mx) {
		if (!sleeping_time.empty()) {
			std::this_thread::sleep_for(chrono::milliseconds(sleeping_time[i]));
		}
		
		scoped_lock<mutex> lk(mx);
		duration += dur;
	}

	template <class Duration, class Container, class Produce, class Consume, class Pred, class DataIngestor, class... ContainerArgs>
	pair<bool, Duration> testContainerProducerConsumer(Produce&& produce, Consume&& consume,
		Pred&& postCondition, DataIngestor get_next_data, array<size_t, 3> setup_arg,
		array<vector<size_t>, 2> sleeping_time,
		ContainerArgs&&... container_args) {

		auto [produce_n, consume_n, threads_num] = setup_arg;
		unique_ptr<Container> c = make_unique<Container>(forward<ContainerArgs>(container_args)...);

		vector<thread> threads(2 * threads_num);
		mutex mx;
		Duration duration{};

		for (size_t i = 0; i < threads_num; ++i) {
			threads[i] = thread([&] {
				for (size_t j = 0; j < produce_n; ++j) {
					auto dur = successTimeFuncInvocation<Duration>(forward<Produce>(produce), c.get(), get_next_data());

					sleepAddDuration(j, sleeping_time[0], duration, dur, mx);
				}
			});

			threads[i + threads_num] = thread([&] {
				for (size_t j = 0; j < consume_n; ++j) {
					auto dur = successTimeFuncInvocation<Duration>(forward<Consume>(consume), c.get());

					sleepAddDuration(j, sleeping_time[1], duration, dur, mx);
				}
			});
		}

		for (size_t i = 0; i < threads.size(); ++i)
			threads[i].join();

		bool res = postCondition(c.get());
		duration += timeFuncInvocation([&] {
			c.reset();
			return true;
		}).second;

		return { res, duration };
	}

	template <class Duration, class Container, class Produce, class Consume, class Pred, class DataIngestor, class... Args>
	vector<pair<bool, Duration>> getTestResults(Produce&& produce, Consume&& consume,
		Pred&& pred, DataIngestor&& get_next_data, vector<array<size_t, 3>> setup_args,
		vector<array<vector<size_t>, 2>> sleeping_times,
		bool verbose, Args&&... args) {

		vector<pair<bool, Duration>> res(setup_args.size());

		if (verbose) {
			cout << "Running tests for " << typeid(Container).name() << "...\n";
		}

		for (size_t i = 0; i < res.size(); ++i) {
			res[i] = testContainerProducerConsumer<Duration, Container>(
				forward<Produce>(produce),
				forward<Consume>(consume),
				forward<Pred>(pred),
				forward<DataIngestor>(get_next_data),
				setup_args[i],
				sleeping_times.empty() ? array<vector<size_t>, 2>() : sleeping_times[i],
				forward<Args>(args)...
			);
		}

		if (verbose) {
			cout << setw(10) << "Test num" << setw(15) << "Ingest size";
			cout << setw(15) << "Threads num";
			cout << setw(10) << "Correct" << setw(25) << "Execution time\n";
			for (size_t i = 0; i < res.size(); ++i) {
				cout << setw(10) << "#" + to_string(i);
				cout << setw(15) << to_string((setup_args[i][0] + setup_args[i][1]) * setup_args[i][2]);
				cout << setw(15) << to_string(setup_args[i][2] * 2);
				cout << setw(10) << res[i].first ? "True" : "False";
				cout << setw(25) << res[i].second;
				cout << "\n";
			}
			cout << "\n";
		}

		return res;
	}

	template<typename...>
	struct TypeList {};

	template<typename T>
	using Type = TypeList<T>;

	template<typename F, typename... Ts>
	void for_each(TypeList<Ts...>, F f) {
		(..., f(type_identity<Ts>{}));
	}

	vector<size_t> getRandomInts(size_t n, const pair<size_t, size_t> &min_max_limits) {
		random_device rnd_device;
		mt19937 mersenne_engine{ rnd_device() };
		uniform_int_distribution<size_t> dist{ min_max_limits.first , min_max_limits.second };

		vector<size_t> res(n);
		generate(res.begin(), res.end(), [&] { return dist(mersenne_engine); });
		return res;
	}

	vector<array<size_t, 3>> getDefaultSetupArgs() {
		return {
			{100000, 100000, 4},
			{1000, 1000, 15},
			{1000000, 1000000, 10},
			{5000, 5000, 50},
			{50000, 50000, 7},
			{10000000, 10000000, 2}
		};
	}

	auto getSleepingArgs(const vector<array<size_t, 3>> &setup_args, size_t maxi) {
		vector<array<vector<size_t>, 2>> sleeping_times(setup_args.size());

		pair<size_t, size_t> sleeping_limits = { 0, maxi };
		for (size_t i = 0; i < setup_args.size(); ++i) {
			sleeping_times[i] = {
				testing::getRandomInts(setup_args[i][0], sleeping_limits),
				testing::getRandomInts(setup_args[i][1], sleeping_limits)
			};
		}

		return std::make_pair(setup_args, sleeping_times);
	}
}

#include <boost/lockfree/stack.hpp>
#include <boost/lockfree/queue.hpp>

void testStacks() {
	using Duration = chrono::duration<double, ratio<1, 1000>>;

	testing::for_each(testing::TypeList<
		stacks::AtomicSharedStack<int>,
		stacks::AtomicReclaimStack<int>,
		stacks::AtomicRefCountStack<int>,
		stacks::LockBasedStack<int, mutex>,
		stacks::LockBasedStack<int, locks::SpinLock>,
		stacks::unsafe::AtomicLeakStack<int>
	>{}, [&](auto arg) {

		using Container = decltype(arg)::type;

		testing::getTestResults<Duration, Container>(
			[&](Container* c, int val) { c->push(val); return true; },
			[&](Container* c) { return c->pop(); },
			[&](Container* c) { return c->empty(); },
			[] { return rand(); },
			testing::getDefaultSetupArgs(),
			{},
			true
		);
	});

	testing::for_each(testing::TypeList<boost::lockfree::stack<int>> {}, [&](auto arg) {
		using Container = decltype(arg)::type;

		testing::getTestResults<Duration, Container>(
			[&](Container* c, int val) { c->push(val); return true; },
			[&](Container* c) { int val;  return c->pop(val); },
			[&](Container* c) { return c->empty(); },
			[] { return rand(); },
			testing::getDefaultSetupArgs(),
			{},
			true,
			0
		);
	});
}

void testQueues() {
	using Duration = chrono::duration<double, ratio<1, 1000>>;

	testing::for_each(testing::TypeList <
		//queues::AtomicReclaimQueue<int>,
		queues::AtomicSharedQueue<int>,
		queues::TwoLockQueue<int, std::mutex>,
		queues::TwoLockQueue<int, locks::SpinLock>,
		queues::LockBasedQueue<int, std::mutex>,
		queues::LockBasedQueue<int, locks::SpinLock>,
		queues::unsafe::AtomicLeakQueue<int>
		>
		{}
		, [&](auto arg) {

		using Container = decltype(arg)::type;

		testing::getTestResults<Duration, Container>(
			[&](Container* c, int val) { c->push(val); return true; },
			[&](Container* c) { return c->pop(); },
			[&](Container* c) { return c->empty(); },
			[] { return rand(); },
			testing::getDefaultSetupArgs(),
			{},
			true
		);
	});

	testing::for_each(testing::TypeList<boost::lockfree::queue<int>>{}, [&](auto arg) {
		using Container = decltype(arg)::type;

		testing::getTestResults<Duration, Container>(
			[&](Container* c, int val) { c->push(val); return true; },
			[&](Container* c) { int val;  return c->pop(val); },
			[&](Container* c) { return c->empty(); },
			[] { return rand(); },
			testing::getDefaultSetupArgs(),
			{},
			true,
			0
		);
	});

}

int main() {
	srand(unsigned(time(nullptr)));

	testStacks();
	testQueues();

	return 0;
}