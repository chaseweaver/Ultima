#ifndef QUEUE_H
#define QUEUE_H

#ifdef _MSC_VER
#pragma once
#endif 

#include <queue>
#include <mutex>
#include <exception>
#include <condition_variable>

template<typename T>
class Queue {
	std::queue<T> queue_;
  mutable std::mutex m_;
  std::condition_variable data_cond_;

  Queue& operator=(const Queue&) = delete;
  Queue(const Queue& other) = delete;

public:

	/*
	 * Queue::Queue()
	 * Default construtor.
	 */ 
	Queue() {}

	/*
	 * Queue::~Queue()
	 * Default deconstrutor.
	 */
	~Queue() {
		while (!queue_.empty())
			queue_.pop();
	}

	/*
	 * Queue::push(T)
	 * Locks the mutes and pushes an item into the queue.
	 * Inlcudes a new scope for the mutex object.
	 */
	void push(T item) {
		{
			std::lock_guard<std::mutex> lock(m_);
			queue_.push(std::move(item));
		}
		data_cond_.notify_one();
	}

	/*
	 * Queue::try_and_pop(T&)
	 * Trys to pop an item from the queue. Stores popped item address
	 * in the Template object. Returns false if unable.
	 */
	bool try_and_pop(T& item) {
		std::lock_guard<std::mutex> lock(m_);
		if (queue_.empty())
			return false;
		item = std::move(queue_.front());
		queue_.pop();
		return true;
	}

	/*
	 * Queue::wait_and_pop(T&)
	 * Trys to pop an item from the queue, if available. If not, wait and try again.
	 * Stores popped item address in the Template object.
	 */
	void wait_and_pop(T& item) {
		std::unique_lock<std::mutex> lock(m_);
		while (queue_.empty())
			data_cond_.wait(lock);
		item = std::move(queue_.front());
		queue_.pop();
	}

	/*
	 * Queue::wait_and_pop()
	 * Trys to pop an item from the queue, if available. If not, wait and try again.
	 */
	void wait_and_pop() {
		std::unique_lock<std::mutex> lock(m_);
		while (queue_.empty())
			data_cond_.wait(lock);
		std::move(queue_.front());
		queue_.pop();
	}

	/*
	 * Queue::front()
	 * Returns the front of the queue.
	 */
	T front() {
		std::unique_lock<std::mutex> lock(m_);
		while (queue_.empty())
			data_cond_.wait(lock);
		return queue_.front();
	}

	/*
	 * Queue::back()
	 * Returns the back of the queue.
	 */
	T back() {
		std::unique_lock<std::mutex> lock(m_);
		while (queue_.empty())
			data_cond_.wait(lock);
		return queue_.back();
	}

	/*
	 * Queue::empty()
	 * Returns true if queue is empty, false otherwise.
	 */
	bool empty() const {
		std::lock_guard<std::mutex> lock(m_);
		return queue_.empty();
	}

	/*
	 * Queue::size()
	 * Returns the sie of the queue.
	 */
	unsigned size() const {
		std::lock_guard<std::mutex> lock(m_);
		return queue_.size();
	}
};

#endif