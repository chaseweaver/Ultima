#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#ifdef _MSC_VER
#pragma once
#endif 

#include <queue>
#include <mutex>
#include <exception>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
	std::queue<T> q;
  mutable std::mutex m;
  std::condition_variable cond;

  ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
  ThreadSafeQueue(const ThreadSafeQueue& other) = delete;

public:

	/*
	 * ThreadSafeQueue::ThreadSafeQueue()
	 * Default construtor.
	 */ 
	ThreadSafeQueue() {}

	/*
	 * ThreadSafeQueue::~ThreadSafeQueue()
	 * Default deconstrutor.
	 */
	~ThreadSafeQueue() {
		while (!q.empty())
			q.pop();
	}

	/*
	 * ThreadSafeQueue::push(T)
	 * Locks the mutes and pushes an item into the queue.
	 * Inlcudes a new scope for the mutex object.
	 */
	void push(T item) {
		{
			std::lock_guard<std::mutex> lock(m);
			q.push(std::move(item));
		}
		cond.notify_one();
	}

	/*
	 * ThreadSafeQueue::try_and_pop(T&)
	 * Trys to pop an item from the queue. Stores popped item address
	 * in the Template object. Returns false if unable.
	 */
	bool try_and_pop(T& item) {
		std::lock_guard<std::mutex> lock(m);
		if (q.empty())
			return false;
		item = std::move(q.front());
		q.pop();
		return true;
	}

	/*
	 * ThreadSafeQueue::wait_and_pop(T&)
	 * Trys to pop an item from the queue, if available. If not, wait and try again.
	 * Stores popped item address in the Template object.
	 */
	void wait_and_pop(T& item) {
		std::unique_lock<std::mutex> lock(m);
		while (q.empty())
			cond.wait(lock);
		item = std::move(q.front());
		q.pop();
	}

	/*
	 * ThreadSafeQueue::wait_and_pop()
	 * Trys to pop an item from the queue, if available. If not, wait and try again.
	 */
	void wait_and_pop() {
		std::unique_lock<std::mutex> lock(m);
		while (q.empty())
			cond.wait(lock);
		std::move(q.front());
		q.pop();
	}

	/*
	 * ThreadSafeQueue::front()
	 * Returns the front of the queue.
	 */
	T front() {
		std::unique_lock<std::mutex> lock(m);
		while (q.empty())
			cond.wait(lock);
		return q.front();
	}

	/*
	 * ThreadSafeQueue::back()
	 * Returns the back of the queue.
	 */
	T back() {
		std::unique_lock<std::mutex> lock(m);
		while (q.empty())
			cond.wait(lock);
		return q.back();
	}

	/*
	 * ThreadSafeQueue::empty()
	 * Returns true if queue is empty, false otherwise.
	 */
	bool empty() const {
		std::lock_guard<std::mutex> lock(m);
		return q.empty();
	}

	/*
	 * ThreadSafeQueue::size()
	 * Returns the sie of the queue.
	 */
	unsigned size() const {
		std::lock_guard<std::mutex> lock(m);
		return q.size();
	}
};

#endif