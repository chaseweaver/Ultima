#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#ifdef _MSC_VER
#pragma once
#endif 

#include <iostream>
#include <string>

template <class T>
class Node {
public:
	T data;
	Node<T>* next;
	T get_data(void) { return data; }
	Node<T>* get_next(void) { return next; }
	void set_data(T dat) { data = dat; }
	void set_next(Node<T>* dat) { next = dat; }
};

template <class T>
class LinkedList {
private:
	Node<T>* head;
	Node<T>* tail;

public:

	/*
	* LinkedList::LinkedList()
	* Default constructor.
	*/ 
	LinkedList() {
		head = NULL;
	}

	/*
	* LinkedList::~LinkedList()
	* Default deconstructor.
	*/ 
	~LinkedList() {
		Node<T>* tmp = new Node<T>();
		Node<T>* ret = new Node<T>();
		tmp = head;
		if (tmp == NULL)
			return;
		while (tmp) {
			ret = tmp->get_next();
			delete tmp;
			tmp = ret;
		}
	}

	/*
	* LinkedList::add(T)
	* Adds a template item to the list
	*/ 
	void add(T dat) {	
		Node<T>* tmp = new Node<T>();
		Node<T>* ret = new Node<T>();

		if (head == NULL) {
			head = new Node<T>();
			head->set_data(dat);
			head->set_next(NULL);
			return;
		} 

		tmp = head;
		while (tmp->get_next() != NULL) 
			tmp = tmp->get_next();

		ret = new Node<T>();
		ret->set_data(dat);
		ret->set_next(NULL);
		tmp->set_next(ret);
	}

	/*
	* LinkedList::remove(T)
	* Removes a template item from the list.
	*/ 
	void remove(T dat) {
		Node<T>* tmp = new Node<T>();
		Node<T>* ret = new Node<T>();

		if (head == NULL)
			return;

		tmp = head;
		while (tmp) {
			if (tmp->get_data() == dat) {
				ret->set_next(tmp->get_next());
				delete tmp;
				return;
			}

			ret = tmp;
			tmp = tmp->get_next();
		}
	}

	/*
	* LinkedList::find(T)
	* If exists, returns the template element in the list.
	*/ 
	T find(T dat) {
		if (head == NULL)
			return head->get_data();

		Node<T>* cur = new Node<T>();
		cur = head;
		Node<T>* prev = new Node<T>();
		prev = head;

		while (cur) {
			if (cur->get_data() == dat)
				return cur->get_data();
			prev = cur;
			cur = cur->get_next();
		}

		return cur->get_data();
	}

	/*
	* LinkedList::remove_front()
	* Removes the first element in the list.
	*/ 
	void remove_front() {
		Node<T>* tmp = new Node<T>();
		if (head == NULL)
			return;

		tmp = head;
		if (tmp) {
			head = tmp->get_next();
			delete tmp;
		}
	}

		/*	
	* LinkedList::return_front()
	* Returns and removes the first element in the list.
	*/ 
	T return_front() {
		if (head == NULL)
			return;

		T dat = head->get_data();
		Node<T>* tmp = new Node<T>();
		head = head->get_next();

		delete tmp;
		return dat;
	}

	/*
	* LinkedList::exists(T)
	* Returns true if the template data exists within the list.
	*/ 
	bool exists(T dat) {
		if (head == NULL)
			return false;

		Node<T>* tmp = new Node<T>();
		tmp = head;
		Node<T>* ret = new Node<T>();
		ret = head;

		while (tmp) {
			if (tmp->get_data() == dat)
				return true;
			ret = tmp;
			tmp = tmp->get_next();
		}

		return false;
	}

	/*
	* LinkedList::reverse()
	* Reverses the order of the list.
	*/ 
	void reverse() {
		if (head == NULL || head->get_next() == NULL) 
			return;

		Node<T>* parent = head;
		Node<T>* me = head->get_next();
		Node<T>* child = me->get_next();
		parent->set_next(NULL);
		
		while (child != NULL) {
			me->set_next(parent);
			parent = me;
			me = child;
			child = child->get_next();
		}

		head = me;
		head->set_next(parent);
	}

	/*
	* LinkedList::reset()
	* Resets the contents of the list.
	*/ 
	void reset() {
		Node<T>* tmp = new Node<T>();
		Node<T>* ret = new Node<T>();

		if (head == NULL)
			return;

		tmp = head;
		while (tmp) {
			ret = tmp->get_next();
			delete tmp;

			if (ret != head) {
				head = NULL;
				return;
			}

			tmp = ret;
		}
	}

	/*
	* LinkedList::make_circular()
	* Connects the tail node to the head, making the list circular.
	*/ 
	void make_circular() {
		Node<T>* tmp = new Node<T>();
		if (head == NULL || head->next == NULL)
			return;
		tmp = head;
		while (tmp) {
			if (tmp->get_next() == NULL) {
				tmp->set_next(head);
				return;
			}
			tmp = tmp->get_next();
		}
	}

	/*
	* LinkedList::is_circular()
	* Returns true if list is circular, false otherwise
	*/ 
	bool is_circular() {
		Node<T>* tmp = new Node<T>();
		Node<T>* ret = new Node<T>();
		if (head == NULL || head->get_next() == NULL)
			return false;
		tmp = head;
		ret = head;
		while (ret) {
			tmp = tmp->get_next();
			if (ret->get_next() == NULL)
				return false;
			ret = (ret->get_next())->get_next();
			if (tmp == ret)
				return true;
		}
		return false;
	}

	/*
	* LinkedList::print()
	* Prints contents of list using std::cout.
	*/ 
	void print() {
		Node<T>* tmp = new Node<T>();
		if (head == NULL)
			return;

		tmp = head;
		while (tmp) {
			std::cout << tmp->get_data() << std::endl;
			tmp = tmp->get_next();

			if (tmp != NULL) {
				if (tmp == head)
					return;
			}

		}
	}

	/*
	* LinkedList::size()
	* Returns size of the list.
	*/ 
	int size() { 
		Node<T>* tmp = new Node<T>();
		if (head == NULL)
			return 0;

		tmp = head;
		int size = 0;
		while (tmp) {
			size++;
			if(tmp->get_next() == 0) {
				return size; 
			}
			tmp = tmp->get_next();	
		}
		return size;
	}

	/*
	* LinkedList::is_empty()
	* Returns true if list is empty, false otherwise.
	*/ 
	bool is_empty() { return head == NULL; }
};

#endif