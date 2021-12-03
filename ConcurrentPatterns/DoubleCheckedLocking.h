#pragma once

#include <atomic>
#include <mutex>

namespace concurrent::patterns {

	class DoubleCheckSingleton {
		DoubleCheckSingleton() = default;

		static std::atomic<DoubleCheckSingleton*> ins;
		static std::mutex mx;
	
	public:
		static DoubleCheckSingleton* GetInstance() {
			DoubleCheckSingleton* p = ins.load(std::memory_order_acquire);

			if (p == nullptr) { // 1st check
				std::scoped_lock<std::mutex> lock(mx);
				p = ins.load(std::memory_order_relaxed);

				if (p == nullptr) { // 2nd (double) check
					p = new DoubleCheckSingleton();
					ins.store(p, std::memory_order_release);
				}
			}
			return p;
		}
	};

	std::atomic<DoubleCheckSingleton*> DoubleCheckSingleton::ins{ nullptr };
	std::mutex DoubleCheckSingleton::mx{};

	class MeyersSingleton {
		MeyersSingleton() = default;

	public:
		static MeyersSingleton& GetInstance() {
			static MeyersSingleton ins;
			return ins;
		}
	};

	class CallOnceSingleton {
		CallOnceSingleton() = default;

		static std::optional<CallOnceSingleton> ins;
		static std::once_flag flag;
	public:
		static CallOnceSingleton* GetInstance() {
			std::call_once(flag, []() { ins.emplace(CallOnceSingleton{}); });
			return &*ins;
		}
	};

	std::optional<CallOnceSingleton> CallOnceSingleton::ins{};
	std::once_flag CallOnceSingleton::flag{};

}