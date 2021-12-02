#pragma once

#include <semaphore>
#include <mutex>
#include <functional>

namespace concurrent::primitives {

	/*
	When a leader arrives, it checks to see if there is a follower waiting.
	If so, they can both proceed. Otherwise it waits.
	Similarly, when a follower arrives, it checks for a leader and either proceeds or waits, accordingly
	*/

	// Leader-follower signalling model
	class SignalQueue {
		std::counting_semaphore<> leaders_queue, followers_queue;

	public:
		SignalQueue() : leaders_queue(0), followers_queue(0) {}

		void leader(std::function<void ()> leader_action) {
			followers_queue.release();
			leaders_queue.acquire();

			leader_action();
		}

		void follower(std::function<void()> follower_action) {
			leaders_queue.release();
			followers_queue.acquire();

			follower_action();
		}
	};

	// Only one pair execute actions at the same time.
	class ExclusiveSignalQueue {
		std::binary_semaphore mx, rendezvous;
		std::counting_semaphore<> leaders_queue, followers_queue;
		int leaders = 0, followers = 0;

	public:
		ExclusiveSignalQueue() : mx(1), rendezvous(0), leaders_queue(0), followers_queue(0) {}

		// leader releases the mutex
		void leader(std::function<void()> leader_action) {
			mx.acquire();
			if (followers > 0) {
				--followers;
				followers_queue.release();
			}
			else {
				++leaders;
				mx.release();
				leaders_queue.acquire();
			}

			leader_action();

			rendezvous.acquire();
			mx.release();
		}

		void follower(std::function<void()> follower_action) {
			mx.acquire();
			if (leaders > 0) {
				--leaders;
				leaders_queue.release();
			}
			else {
				++followers;
				mx.release();
				followers_queue.acquire();
			}

			follower_action();

			rendezvous.release();
		}
	};

}