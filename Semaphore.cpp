#include "inc/Semaphore.h"


/*
 * Semaphore::Semaphore(std::string, int)
 * Default constructor.
 */
Semaphore::Semaphore(std::string name, int val)
	: resource_name(name), value(val) {}

/*
 * Semaphore::~Semaphore()
 * Default deconstructor.
 */
Semaphore::~Semaphore() {}

/*
 * Semaphore::wait()
 * Locks threads depending on state.
 */
void Semaphore::wait() {
	std::unique_lock<std::mutex> lock(mutex);

	if (--value < 0) {
		do {
			cond.wait(lock);
		} while (wakeups < 1);
		--wakeups;
	}
}

/*
 * Semaphore::try_wait()
 * Returns whether or not an unlock can happen.
 */
bool Semaphore::try_wait() {
	std::unique_lock<std::mutex> lock(mutex);

	if (value > 0) {
		--value;
		return true;
	} else {
		return false;
	}
}

/*
 * Semaphore::signal()
 * Signals to other threads that one has finished.
 */
void Semaphore::signal() {
	std::unique_lock<std::mutex> lock(mutex);

	if (++value <= 0) {
		++wakeups;
		cond.notify_one();
	}
}