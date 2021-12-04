#pragma once

#include <semaphore>
#include "LightSwitch.h"

namespace concurrent::patterns {

	// Readers preference, writers starve.
	class ReadersWritersR {
		std::binary_semaphore room_empty;
		LightSwitch read_switch;

	public:
		ReadersWritersR(): room_empty(1), read_switch(
			[this] { room_empty.acquire(); }, [this] { room_empty.release(); })
		{}

		void shared_lock() { read_switch.lock(); }
		void shared_unlock() { read_switch.unlock(); }

		void lock() { room_empty.acquire(); }
		void unlock() { room_empty.release(); }
	};

	// When a writer arrives, the existing readers can finish, but no additional readers may enter.
	class ReadersWriters {
		std::binary_semaphore turnstile, room_empty;
		LightSwitch read_switch;

	public:
		ReadersWriters() : turnstile(1), room_empty(1), read_switch(
			[this] { room_empty.acquire(); }, [this] { room_empty.release(); })
		{}

		void shared_lock() {
			turnstile.acquire();
			turnstile.release();

			read_switch.lock();
		}
		void shared_unlock() { read_switch.unlock(); }

		void lock() {
			turnstile.acquire();
			room_empty.acquire();
		}
		void unlock() { room_empty.release(); }
	};

	// Once a writer arrives, no readers should be allowed to enter until all writers have left the system.
	class ReadersWritersW {
		std::binary_semaphore readers_block, writers_block;
		LightSwitch read_switch, write_switch;

	public:
		ReadersWritersW() : readers_block(1), writers_block(1),
			read_switch([this] { writers_block.acquire(); }, [this] { writers_block.release(); }),
			write_switch([this] { readers_block.acquire(); }, [this] { readers_block.release(); }) {
		}

		void shared_lock() {
			readers_block.acquire();
			read_switch.lock();
			readers_block.release();
		}
		void shared_unlock() {
			read_switch.unlock();
		}

		void lock() {
			write_switch.lock();
			writers_block.acquire();
		}
		void unlock() {
			writers_block.release();
			write_switch.unlock();
		}
	};

}