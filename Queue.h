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
	Queue() {}

	void push(T item) {
		{
			std::lock_guard<std::mutex> lock(m_);
			queue_.push(std::move(item));
		}
		data_cond_.notify_one();
	}

	/// Return immediately, with true if successful retrieval
	bool try_and_pop(T& popped_item) {
		std::lock_guard<std::mutex> lock(m_);
		if (queue_.empty())
			return false;
		popped_item = std::move(queue_.front());
		queue_.pop();
		return true;
	}

	/// Try to retrieve, if no items, wait till an item is available and try again
	void wait_and_pop(T& popped_item) {
		std::unique_lock<std::mutex> lock(m_);
		while (queue_.empty())
			data_cond_.wait(lock);
		popped_item = std::move(queue_.front());
		queue_.pop();
	}

	/// Try to retrieve, if no items, wait till an item is available and try again
	void wait_and_pop() {
		std::unique_lock<std::mutex> lock(m_);
		while (queue_.empty())
			data_cond_.wait(lock);
		std::move(queue_.front());
		queue_.pop();
	}

	T front() {
		std::unique_lock<std::mutex> lock(m_);
		while (queue_.empty())
			data_cond_.wait(lock);
		return queue_.front();
	}

	T back() {
		std::unique_lock<std::mutex> lock(m_);
		while (queue_.empty())
			data_cond_.wait(lock);
		return queue_.back();
	}

	bool empty() const {
		std::lock_guard<std::mutex> lock(m_);
		return queue_.empty();
	}

	unsigned size() const {
		std::lock_guard<std::mutex> lock(m_);
		return queue_.size();
	}
};

#endif