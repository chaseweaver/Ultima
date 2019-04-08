#ifndef QUEUE_H
#define QUEUE_H

#ifdef _MSC_VER
#pragma once
#endif

#include <cstddef>
#include <cstdio>
#include <iostream>

#include <condition_variable>
#include <mutex>
#include <queue>

template < class T > class Node {
  public:
  T value;
  Node* next;

  /*
   * Node::get_value(void)
   * Returns the value at a node in the queue.
   */
  T get_value(void) { return value; }

  /*
   * Node::*get_next(void)
   * Next value in line of queue.
   */
  Node* get_next(void) { return next; }

  /*
   * Node::set_value(T)
   * Sets the node value with a template value (any kind).
   */
  void set_value(T item) { value = item; }

  /*
   * Node::set_next(T)
   * Sets the next value in line of queue.
   */
  void set_next(Node< T >* item) { next = item; }
};

template < class T > class Queue {
  private:
  Node< T >* head;
  Node< T >* tail;
  int count;

  public:
  Queue(void) {
    head = NULL;
    tail = NULL;
    count = 0;
  }

  /*
   * Queue::Queue(const Queue&)
   * Default copy constructor.
   */
  Queue(const Queue& other) {
    head = NULL;
    tail = NULL;
    count = 0;
    Node< T >* p = other.head;
    while (p != NULL) {
      enqueue(p->get_value());
      tail->set_value(p->get_value());
      p = p->next;
    }
  }

  /*
   * Queue::~Queue(void)
   * Default deconstructor.
   */
  ~Queue(void) {
    while (count != 0) dequeue();
  }

  /*
   * Queue::enqueue(T)
   * Enqueue generic datum into queue and increment the size.
   */
  void enqueue(T item) {
    Node< T >* tmp = new Node< T >();
    tmp->set_value(item);
    tmp->set_next(NULL);

    if (count == 0)
      head = tail = tmp;
    else {
      tail->set_next(tmp);
      tail = tmp;
    }
    count++;
  }

  /*
   * Queue::dequeue(void)
   * Dequeue the first element in the queue off.
   */
  T dequeue(void) {
    if (empty()) return 0;

    T val = head->get_value();
    Node< T >* tmp = head;
    head = head->get_next();
    count--;

    delete tmp;
    return val;
  }

  /*
   * Queue::dequeue(T&)
   * Dequeue the first element in the queue off.
   */
  void dequeue(T& item) {
    if (empty()) return;

    item = head->get_value();
    Node< T >* tmp = head;
    head = head->get_next();
    count--;

    delete tmp;
  }

  /*
   * Queue::move_to_back()
   * A wrapper for the dequeue(T&) and enqueue(T&) operations.
   */
  void move_to_back() {
    T item;
    dequeue(item);
    enqueue(item);
  }

  /*
   * Queue::size(void) const
   * Returns the size of the queue.
   */
  unsigned size(void) const { return count; }

  /*
   * Queue::front(void)
   * Returns front of the queue.
   */
  T front(void) {
    if (empty()) return;

    T ret = head->get_value();
    return ret;
  }

  /*
   * Queue::back(void)
   * Returns back of the queue.
   */
  T back(void) {
    if (empty()) return;

    T ret = tail->get_value();
    return ret;
  }

  /*
   * Queue::front(T&)
   * Returns front of queue.
   */
  void front(T& item) {
    if (empty()) return;

    item = head->get_value();
  }

  /*
   * Queue::back(T&)
   * Returns back of the queue.
   */
  void back(T& item) {
    if (empty()) return;

    item = tail->get_value();
  }

  /*
   * Queue::empty(void) const
   * Checks if the queue is empty.
   */
  bool empty(void) const { return count == 0 ? true : false; }
};

#endif