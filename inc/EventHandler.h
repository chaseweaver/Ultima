#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#ifdef _MSC_VER
#pragma once
#endif 

#include "Queue.h"
#include <iostream>
#include <chrono>

class EventHandler {
private:
	struct EVENT {
		int event_id;
		time_t timestamp;
		std::string log;
	};

	Queue<EVENT> l;
	int id = 0;

public:
	EventHandler();
	~EventHandler();
	void add_event(std::string);
	void add_event(int event_id, std::string);
	void add_event(int event_id, time_t, std::string);
	int event_count();
	void list_events();
	bool no_events();
};

#endif