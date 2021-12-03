#pragma once

#include <mutex>
#include <semaphore>
#include <queue>
#include <condition_variable>

namespace concurrent::patterns {

	template <class Event, class Queue = std::queue<Event>, class Notifier = std::counting_semaphore<>>
	class FiniteProducerConsumer;

	template <class Event, class Queue>
	class FiniteProducerConsumer<Event, Queue, std::counting_semaphore<>> {
		std::counting_semaphore<> item_added, spaces;
		std::mutex mx;
		Queue events;

	public:
		FiniteProducerConsumer(size_t n) : item_added(0), spaces(n) {}

		void produce(Event event) {
			spaces.acquire();

			{
				std::scoped_lock<std::mutex> lk(mx);
				events.push(std::move(event));
			}

			item_added.release();
		}

		Event consume() {
			Event event;
			item_added.acquire();

			{
				std::scoped_lock<std::mutex> lk(mx);
				event = std::move(events.front());
				events.pop();
			}

			spaces.release();
			return event;
		}
	};

	template <class Event, class Queue>
	class FiniteProducerConsumer<Event, Queue, std::condition_variable> {
		std::condition_variable item_added, item_popped;
		std::mutex mx;
		size_t max_spaces;
		Queue events;

	public:
		FiniteProducerConsumer(size_t spaces) : max_spaces(spaces) {}

		void produce(Event event) {
			std::unique_lock<std::mutex> lk(mx);
			item_popped.wait(lk, [&] { return events.size() != max_spaces; });

			events.push(std::move(event));
			if (events.size() == 1) item_added.notify_one();
		}

		Event consume() {
			std::unique_lock<std::mutex> lk(mx);
			item_added.wait(lk, [&] { return !events.empty(); });

			Event event = std::move(events.front());
			events.pop();

			if (events.size() + 1 == max_spaces) item_popped.notify_one();
			return event;
		}
	};

}