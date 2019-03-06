#include "../inc/EventHandler.h"
#include <iostream>
#include <chrono>

/*
 * EventHandler::EventHandler()
 * Default constructor
 */ 
EventHandler::EventHandler(){}

/*
 * EventHandler::!EventHandler()
 * Default deconstructor
 */ 
EventHandler::~EventHandler(){}

/*
 * EventHandler::add_event(std::string)
 * Adds events to the list with current list size and epoch time.
 */ 
void EventHandler::add_event(std::string log) {
	EVENT e;
	e.event_id = ++id;
	e.timestamp = std::time(0);
	e.log = log;
	l.push(e);
}

/*
 * EventHandler::add_event(int, std::string)
 * Adds events to the list with current epoch time.
 */ 
void EventHandler::add_event(int event_id, std::string log) {
	++id;
	EVENT e;
	e.event_id = event_id;
	e.timestamp = std::time(0);
	e.log = log;
	l.push(e);
}

/*
 * EventHandler::add_event(int, time_t, std::string)
 * Adds events to the list.
 */ 
void EventHandler::add_event(int event_id, time_t timestamp, std::string log) {
	++id;
	EVENT e;
	e.event_id = event_id;
	e.timestamp = timestamp;
	e.log = log;
	l.push(e);
}

/*
 * EventHandler::event_count()
 * Returns amount of events based on id.
 */ 
int EventHandler::event_count() {
	return id;
}

/*
 * EventHandler::list_events()
 * Prints list of events using std::cout.
 */ 
void EventHandler::list_events() {
	// l.print();
}

/*
 * EventHandler::no_events()
 * Returns true if there are no events in the list.
 */ 
bool EventHandler::no_events() {
	return id == 0;
}