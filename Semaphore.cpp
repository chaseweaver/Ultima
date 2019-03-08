#include "inc/Semaphore.h"


Semaphore::Semaphore(int val) : value(val) {}

void Semaphore::wait() {
	std::unique_lock<std::mutex> lock(mutex);

	if (--value < 0) {
		do {
			cond.wait(lock);
		} while (wakeups < 1);
		--wakeups;
	}
}

bool Semaphore::trywait() {
	std::unique_lock<std::mutex> lock(mutex);

	if (value > 0) {
		--value;
		return true;
	} else {
		return false;
	}
}

void Semaphore::signal() {
	std::unique_lock<std::mutex> lock(mutex);

	if (++value <= 0) {
		++wakeups;
		cond.notify_one();
	}
}