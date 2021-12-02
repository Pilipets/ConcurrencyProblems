#pragma once

#include <mutex>
#include <semaphore>
#include <queue>
#include <condition_variable>

namespace concurrent::primitives {

	template <class Event, class Notifier = std::counting_semaphore<>>
	class ProducerConsumer;

	template <class Event>
	class ProducerConsumer<Event, std::counting_semaphore<>> {
		std::counting_semaphore<> item_added;
		std::mutex mx;
		std::queue<Event> events;

	public:
		ProducerConsumer() : item_added(0) {}

		void produce(Event event) {
			{
				std::scoped_lock<std::mutex> lk(mx);
				events.push(std::move(event));
			}
			item_added.release();
		}

		Event consume() {
			item_added.acquire();
			
			std::scoped_lock<std::mutex> lk(mx);
			Event event = std::move(events.front());
			events.pop();
			return event;
		}
	};

	template <class Event>
	class ProducerConsumer<Event, std::condition_variable> {
		std::condition_variable item_added;
		std::mutex mx;
		std::queue<Event> events;

	public:
		void produce(Event event) {
			{
				std::scoped_lock<std::mutex> lk(mx);
				events.push(std::move(event));
			}
			item_added.notify_one();
		}

		Event consume() {
			std::unique_lock<std::mutex> lk(mx);
			item_added.wait(lk, [&] { return !events.empty(); });

			Event event = std::move(events.front());
			events.pop();
			return event;
		}
	};

}