#ifndef QUEUE_H
#define QUEUE_H

#ifdef _MSC_VER
#pragma once
#endif 

#include <cstddef>
#include <cstdio>
#include <iostream>

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class Node {
public:
	T value;
	Node* next;

	// Returns the value at a node in the queue
	T get_value(void) {
		return value;
	}

	// Next value in line of queue
	Node* get_next(void) {
		return next;
	}

	// Sets the node value with a template value (any kind)
	void set_value(T dat) {
		value = dat;
	}

	// Sets the next value in line of queue
	void set_next(Node<T>* dat) {
		next  = dat;
	}
};

template <class T>
class Queue {
private:
	Node<T>* head;
	Node<T>* tail;
	int count;

public:
	Queue(void) {
		head = NULL;
		tail = NULL;
		count = 0;
	}

	// Copy constructor
	Queue(const Queue& other) {
		head = NULL;
		tail = NULL;
		count = 0;
		Node<T>* p = other.head;
		while(p != NULL) {
			enqueue(p->get_value());
			tail->set_value(p->get_value());
			p = p->next;
		}
	}

	// Deconstructor
	~Queue(void) {
		while (count != 0)
			dequeue();
	}

	// Enqueue generic datum into queue and increment the size
	void enqueue(T ele) {
		Node<T>* tmp = new Node<T>();
		tmp -> set_value(ele);
		tmp -> set_next(NULL);

		if (count == 0)
			head = tail = tmp;
		else {
			tail -> set_next(tmp);
			tail = tmp;
		}
		count++;
	}

	// Dequeue the first element in the queue off
	T dequeue(void) {
		if (empty())
			return 0;

		T val = head -> get_value();
		Node<T>* tmp = head;
		head = head -> get_next();
		count--;
		
		delete tmp;
		return val;
	}

	// Dequeue the first element in the queue off
	void dequeue(T& item) {
		if (empty())
			return;

		item = head -> get_value();
		Node<T>* tmp = head;
		head = head -> get_next();
		count--;
		
		delete tmp;
	}

	// Returns the size of the queue
	unsigned size(void) const {
		return count;
	}

	// Returns front of queue
	T front(void) {
		if (empty())
			return;

		T ret = head -> get_value();
		return ret;
	}

	// Returns back of queue
	T back(void) {
		if (empty())
			return;

		T ret = tail -> get_value();
		return ret;
	}

	// Returns front of queue
	void front(T& item) {
		if (empty())
			return;

		item = head -> get_value();
	}

	// Returns back of queue
	void back(T& item) {
		if (empty())
			return;

		item = tail -> get_value();
	}

	// Checks if queue is empty
	bool empty(void) const {
		return count == 0 ? true : false;
	}
};

#endif