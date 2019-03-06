#ifndef IPC_H
#define IPC_H

#include "Struct.h"
#include "Semaphore.h"
#include "Queue.h"
#include <iostream>
#include <assert.h>
#include <unistd.h>

class IPC {
	private:
		Semaphore* sema = new Semaphore("IPC", 1, NULL, NULL);
		Queue<tcb*> message_box_list;
		bool enabled;

		void monitor_messages() {
			do {
				
				sleep(1);
			} while (enabled);
		}

		// Starts message monitor in new thread
		static void* start_message_monitor(void* p) {
			static_cast<IPC*>(p)->monitor_messages();
			return NULL;
		}

		// Returns the message box for a thread 
		Queue<MESSAGE_TYPE*> fetch_task_control_block_message_box(int destination_task_id) {
			Queue<tcb*> q = Queue<tcb*>(message_box_list);
			sema->down(destination_task_id);

			while (!q.empty()) {
				tcb* _tcb = q.dequeue();
				if (_tcb->id == destination_task_id)
					return _tcb->messages;
			}
		}

	public:
		IPC() {
			assert(!pthread_create(new pthread_t, NULL, start_message_monitor, this));
		}
		~IPC(){}

		// Sends a message between threads
		int message_send(MESSAGE_TYPE* message) {
			sema->down(message->source_task_id);
			fetch_task_control_block_message_box(message->destination_task_id).enqueue(message);
			sema->up();
			return 1;
		}
		
		// Returns the count of all message boxes
		int message_count() {
			sema->down(-1);
			Queue<tcb*> q = Queue<tcb*>(message_box_list);
			int message_count = 0;

			while (!q.empty())
				message_count += q.dequeue()->messages.size();

			sema->up();
			return message_count;
		}

		// Returns messages in calling message box.
		int message_count(int task_id) {
			sema->down(task_id);
			Queue<tcb*> q = Queue<tcb*>(message_box_list);

			while (!q.empty()) {
				tcb* _tcb = q.dequeue();

				if (_tcb->id == task_id)
					return _tcb->messages.size();
			}
		}

		// Enables message-search loop
		void enable() {
			enabled = true;
		}

		// Disables message-search loop
		void disable() {
			enabled = false;
		}

		// Creates a new message box for the thread
		void create_new_message_box(tcb* _tcb) {
			sema->down(_tcb->id);
			message_box_list.enqueue(_tcb);
			sema->up();
		}

		// Creates a new message
		MESSAGE_TYPE* compose_message(tcb* _tcb, int destination_task_id, std::string message) {
			MESSAGE_TYPE* message_type = new MESSAGE_TYPE;
			message_type->source_task_id = _tcb->id;
			message_type->destination_task_id = destination_task_id;
			message_type->msg = message;
			message_type->message_size = message.size();
			return message_type;
		}
};

#endif