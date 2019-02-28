#include <iostream>

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
			std::cout << stderr << std::endl;

		T val = head -> get_value();
		Node<T>* tmp = head;
		head = head -> get_next();
		count--;
		delete tmp;
		return val;
	}

	// Returns the size of the queue
	int size(void) {
		return count;
	}

	// Returns front of queue
	T front(void) {
		if (empty())
			std::cout << stderr << std::endl;

		T ret = head -> get_value();
		return ret;
	}

	// Returns back of queue
	T back(void) {
		if (empty())
			std::cout << stderr << std::endl;

		T ret = tail -> get_value();
		return ret;
	}

	// Checks if queue is empty
	bool empty(void) {
		return count == 0 ? true : false;
	}
};