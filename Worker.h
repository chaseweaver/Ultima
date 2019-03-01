#ifndef WORKER_H
#define WORKER_H

#ifdef _MSC_VER
#pragma once
#endif 

#include "UI.h"
#include <pthread.h>
#include <iostream>

class Worker {
private:
	//UI* ui;
	
public:
	struct ARGUMENTS {
		int id;
		int thread_results;
	};

	/*
	 * Worker::Worker(UI*)
	 * Default constructor. 
	 */ 
	//Worker(UI* u) : ui(u) {}
	Worker(){}
	/*
	 * Worker::~Worker()
	 * Default deconstructor. 
	 */ 
	~Worker() {}

	/*
	 * Worker::start()
	 * Starts worker function. 
	 */ 
	static void* start(void* p) {
		static_cast<Worker*>(p)->work;
		return NULL;
	}

	ARGUMENTS* create_arguments(int id, int thread_results) {
		ARGUMENTS* args = new ARGUMENTS;
		args->id = id;
		args->thread_results = thread_results;
		return args;
	}

	static void work(void* a) {
		ARGUMENTS* args = (ARGUMENTS *) a;

		// sema.down();
		// ui->write(4, 2, 2, "PENIS");
		// sema.up();
	}
};

#endif