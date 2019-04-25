#include "../inc/MasterControlBlock.h"

void mcb_init();
void window_init();

// Unit testing functions
void* unit_test_memory_manager(void*);
void* unit_test_ufs(void*);
void* unit_test_ipc(void*);
void* worker_function(void*);

MASTER_CONTROL_BLOCK* mcb = new MASTER_CONTROL_BLOCK;

int main(int argc, char** argv) {

  mcb_init();
  window_init();

  // Spawn child workers
  for (int i = 1; i <= 8; i++) {
    std::string id = std::to_string(i);
    if (argc > 1 && std::string(argv[1]) == "test") {
      if (i >= 1 && i <= 2) {
        mcb->ui->create_window_lock_spawn(" Mem. Man. #" + id + ' ',
          2,
          0,
          i,
          19,
          10,
          3 + ((i - (i <= 4 ? 1 : 5)) * 20),
          (i <= 4 ? 14 : 24));
        mcb->scheduler->create_new_task("Mem. Man. #" + id,
          unit_test_memory_manager,
          mcb->scheduler->create_arguments(i, 0));
      } else if (i >= 3 && i <= 5) {
        mcb->ui->create_window_lock_spawn(" UFS Man. #" + id + ' ',
          2,
          0,
          i,
          19,
          10,
          3 + ((i - (i <= 4 ? 1 : 5)) * 20),
          (i <= 4 ? 14 : 24));
        mcb->scheduler->create_new_task(
          "UFS Man. #" + id, unit_test_ufs, mcb->scheduler->create_arguments(i, 0));
      } else {
        mcb->ui->create_window_lock_spawn(" IPC Man. #" + id + ' ',
          2,
          0,
          i,
          19,
          10,
          3 + ((i - (i <= 4 ? 1 : 5)) * 20),
          (i <= 4 ? 14 : 24));
        mcb->scheduler->create_new_task(
          "IPC Man. #" + id, unit_test_ipc, mcb->scheduler->create_arguments(i, 0));
      }
    } else {
      mcb->ui->create_window_lock_spawn(" Worker #" + id + ' ',
        4,
        0,
        i,
        19,
        10,
        3 + ((i - (i <= 4 ? 1 : 5)) * 20),
        (i <= 4 ? 14 : 24));
      mcb->scheduler->create_new_task(
        "Worker #" + id, worker_function, mcb->scheduler->create_arguments(i, 0));
    }
  }

  mcb->menu->wait();
  return 0;
}

/*
 * Ultima::mcb_init()
 * Initializes the MASTER_CONTROL_BLOCK objects.
 */
void mcb_init() {
  mcb->ui_sema = new Semaphore(mcb, "UI Handler", 1);
  mcb->sch_sema = new Semaphore(mcb, "Scheduler Handler", 1);
  mcb->log_sema = new Semaphore(mcb, "Logger Handler", 1);
  mcb->tcb_sema = new Semaphore(mcb, "TCB Locker", 1);
  mcb->ipc_sema = new Semaphore(mcb, "IPC Handler", 1);
  mcb->scheduler = new Scheduler(mcb);
  mcb->ui = new UI(mcb);
  mcb->mem_man = new MemoryManager(1024, 32, '.');
  mcb->logger = new Logger(32);
  mcb->ipc = new IPC(mcb, 8, 8);
  mcb->menu = new Menu(
    mcb, mcb->ui->create_window_lock_spawn(" Menu ", 2, 0, MENU_WINDOW, 58, 12, 83, 34));
  mcb->ufs = new UFS(mcb, "root", 16, 128, '_');
}

/*
 * Ultima::window_init()
 * Creates the initial windows.
 */
void window_init() {
  mcb->ui->create_window_spawn(HEADING_WINDOW, 79, 12, 3, 2);
  mcb->ui->write(HEADING_WINDOW, 26, 2, "ULTIMA 2.0 (Spring 2019)");
  mcb->ui->write(HEADING_WINDOW, 18, 3, "The Washington Redskins (Matt + Chase)");
  mcb->ui->write(HEADING_WINDOW, 2, 6, "$ Starting UI handler...");
  mcb->ui->write_refresh(HEADING_WINDOW, 2, 7, "$ Spawning child threads...");

  mcb->ui->create_window_lock_spawn(" Log ", 2, 0, LOG_WINDOW, 39, 12, 3, 34);
  mcb->ui->create_window_lock_spawn(" State ", 2, 0, STATE_WINDOW, 39, 12, 43, 34);
  mcb->ui->create_window_lock_spawn(
    " Output / Mailbox / Memory Core / File System ", 2, 0, OUTPUT_WINDOW, 94, 32, 83, 2);
  mcb->ui->create_window_lock_spawn(" Input ", 2, 0, INPUT_WINDOW, 35, 12, 142, 34);
  mcb->ui->write(INPUT_WINDOW, "\n");
  mcb->menu->print_menu(MENU_WINDOW);
}

/*
 * Ultima::worker_function(void*)
 * Worker function to run in separate threads.
 * It also tells jokes.
 */
void* worker_function(void* arguments) {
  ARGUMENTS* args = (ARGUMENTS*)arguments;
  TASK_CONTROL_BLOCK* tcb = args->task_control_block;
  int& counter = args->thread_results;
  int tracker;
  int create, open, write, close;

  MESSAGE_LIST* message_list = new MESSAGE_LIST;
  int r = 10 + rand() % 20;

  // Generate a random message
  std::string msg = message_list->message_lists[rand() % 20];

  int num = 1 + rand() % 100;
  do {
    mcb->tcb_sema->wait(tcb);

    while (tcb->task_state == RUNNING) {
      if (counter == num) break;

      if (counter == num / 4) {
        tracker = mcb->mem_man->allocate(msg.length());
        mcb->mem_man->write(tracker, msg);
        mcb->ui->write_refresh(
          LOG_WINDOW, " Alloc. memory for Thread #" + std::to_string(args->id) + "\n");
      }

      if (counter == num / 3) {
        std::string name = "Thread #" + std::to_string(args->id) + ".txt";

        mcb->ui->write_refresh(
          LOG_WINDOW, " Creating file for Thread #" + std::to_string(args->id) + "\n");

        char const* perm = "rw--";
        create = mcb->ufs->create_file(name, msg.length() + 1, perm);
        open = mcb->ufs->open(create, name, 'w');
        if (open != -1) {
          mcb->ui->write_refresh(
            LOG_WINDOW, " Opening file for Thread #" + std::to_string(args->id) + "\n");
          write = mcb->ufs->write_string(create, msg);
        } else {
          mcb->ui->write_refresh(
            LOG_WINDOW, " Failed to create file for Thread #" + std::to_string(args->id));
        }
      }

      if (counter == (num / 3) + 5) {
        mcb->ui->write_refresh(
          LOG_WINDOW, " Closing file for Thread #" + std::to_string(args->id) + "\n");
        close = mcb->ufs->close(open);
      }

      if (counter == num - 20) {
        char const* perm = "rwrw";
        mcb->ufs->change_permission(
          open, "Thread #" + std::to_string(args->id) + ".txt", perm);
        mcb->ui->write_refresh(
          LOG_WINDOW, " Changing perms. for Thread #" + std::to_string(args->id) + "\n");
      }

      if (counter == num / 2) {
        int tmp_rand = 1 + rand() % 8;
        int result =
          mcb->ipc->message_send(mcb->ipc->compose_message(tcb, tmp_rand, msg));

        result == 1 ? mcb->ui->write_refresh(LOG_WINDOW,
                        " Msg sent: Thread #" + std::to_string(args->id) +
                          " -> Thread #" + std::to_string(tmp_rand) + "\n")
                    : mcb->ui->write_refresh(LOG_WINDOW,
                        " Msg failed to send: Thread #" + std::to_string(args->id) +
                          " -> Thread #" + std::to_string(tmp_rand) + "\n");
      }
      mcb->ui->write_refresh(args->id, " Running #" + std::to_string(++counter) + "\n");
      usleep(100000);
    }

    mcb->tcb_sema->signal();

    sleep(1);
  } while (counter != num);

  mcb->mem_man->free(tracker);

  mcb->ui->write_refresh(
    args->id, "\n Thread #" + std::to_string(args->id) + "\n has ended.\n");
  mcb->ui->write_refresh(
    LOG_WINDOW, " Thread #" + std::to_string(args->id) + " has ended.\n");

  mcb->ufs->delete_file(open, "Thread #" + std::to_string(args->id));
  mcb->scheduler->set_state(tcb, DEAD);
  return NULL;
}

/*
 * Ultima::unit_test_ufs(void*)
 * Test function for UFS.
 */
void* unit_test_ufs(void* arguments) {
  ARGUMENTS* args = (ARGUMENTS*)arguments;
  TASK_CONTROL_BLOCK* tcb = args->task_control_block;
  int& counter = args->thread_results;
  int success = 0;
  std::string id = std::to_string(args->id);
  MESSAGE_LIST* message_list = new MESSAGE_LIST;

  // Generate a random message
  std::string msg = message_list->message_lists[rand() % 20];

  // Information log strings
  std::string log_alloc = " > Creating file for Thread #" + id + "\n";
  std::string log_suc = " > Suc. opened file for Thread #" + id + "\n";
  std::string log_fail = " > Failed to create\n file for Thread #" + id + "\n";
  std::string log_suc_del = " > Suc. del. file for Thread #" + id + "\n";
  std::string log_fail_del = " > Failed del. file for Thread #" + id + "\n";
  std::string log_perm_suc = " > Suc. chng. perms. for Thread #" + id + "\n";
  std::string log_perm_fail = " > Fail chng. perms. misc file\n";

  std::string wrk_alloc = " Alloc. mem\n Thread #" + id + "\n\n";
  std::string wrk_suc = " Success opened\n file Thread #" + id + "\n\n";
  std::string wrk_fail = " Failed to create\n file Thread #" + id + "\n\n";
  std::string wrk_suc_del = " Suc. del. file\n for Thread #" + id + "\n\n";
  std::string wrk_fail_del = " Failed del. file\n for Thread #" + id + "\n\n";
  std::string wrk_perm_suc = " Suc. chng.\n perms.\n for Thread #" + id + "\n\n";
  std::string wrk_perm_fail = " Fail chng. perm.\n misc file\n\n";

  // File name
  std::string name = "Thread #" + id + ".txt";

  // Number to iterate to
  int tracker = 40 + rand() % 121;

  // File handle tracker
  int file_handle;

  do {
    mcb->tcb_sema->wait(tcb);
    while (tcb->task_state == RUNNING) {
      if (counter == tracker) break;

      // Create file in Ultima File System
      if (counter == tracker / 4) {
        mcb->ui->write_refresh(LOG_WINDOW, log_alloc);
        mcb->ui->write_refresh(args->id, wrk_alloc);

        char const* perm = "rw--";
        file_handle = mcb->ufs->create_file(name, msg.length() + 1, perm);
        file_handle = mcb->ufs->open(file_handle, name, 'w');

        if (file_handle != -1) {
          mcb->ui->write_refresh(LOG_WINDOW, log_suc);
          mcb->ui->write_refresh(args->id, wrk_suc);
          file_handle = mcb->ufs->write_string(file_handle, msg);
        } else {
          mcb->ui->write_refresh(LOG_WINDOW, log_fail);
          mcb->ui->write_refresh(args->id, wrk_fail);
        }
      }

      // Try to change file permissions for other file
      if (counter == tracker / 3) {
        char const* perm = "rwrw";
        file_handle = mcb->ufs->change_permission(
          file_handle, "Thread #" + std::to_string(args->id + 1) + ".txt", perm);
        if (file_handle != -1) {
          mcb->ui->write_refresh(LOG_WINDOW, log_perm_suc);
          mcb->ui->write_refresh(args->id, wrk_perm_suc);
        } else {
          mcb->ui->write_refresh(LOG_WINDOW, log_perm_fail);
          mcb->ui->write_refresh(args->id, wrk_perm_fail);
        }
      }

      // Change file permissions
      if (counter == tracker / 2) {
        char const* perm = "rwrw";
        mcb->ufs->change_permission(file_handle, "Thread #" + id + ".txt", perm);
        mcb->ui->write_refresh(LOG_WINDOW, log_perm_suc);
        mcb->ui->write_refresh(args->id, wrk_perm_suc);
      }

      // Write to the task's window its current iteration
      mcb->ui->write_refresh(args->id, " Running #" + std::to_string(++counter) + "\n");
      usleep(150000);
    }

    // Release semaphore lock
    mcb->tcb_sema->signal();
    usleep(150000);
  } while (counter != tracker);

  // Signal end of thread
  mcb->ui->write_refresh(args->id, "\n Thread #" + id + "\n has ended.\n\n");
  mcb->ui->write_refresh(LOG_WINDOW, " Thread #" + id + " has ended.\n");

  // Delete thread file, if applicable
  file_handle =
    mcb->ufs->delete_file(file_handle, "Thread #" + std::to_string(args->id) + ".txt");
  if (file_handle == 1) {
    mcb->ui->write_refresh(LOG_WINDOW, log_suc_del);
    mcb->ui->write_refresh(args->id, wrk_suc_del);
  } else {
    mcb->ui->write_refresh(LOG_WINDOW, log_fail_del);
    mcb->ui->write_refresh(args->id, wrk_fail_del);
  }

  // Set thread's state to DEAD
  mcb->scheduler->set_state(tcb, DEAD);
  return NULL;
}

/*
 * Ultima::unit_test_memory_manager(void*)
 * Test function for memory manager.
 */
void* unit_test_memory_manager(void* arguments) {
  ARGUMENTS* args = (ARGUMENTS*)arguments;
  TASK_CONTROL_BLOCK* tcb = args->task_control_block;
  int& counter = args->thread_results;
  int success = 0;
  std::string id = std::to_string(args->id);
  MESSAGE_LIST* message_list = new MESSAGE_LIST;

  // Generate a random message
  std::string msg = message_list->message_lists[rand() % 20];

  // Information strings
  std::string log_alloc = " > Alloc. mem. for Thread #" + id + "\n";
  std::string log_suc = " > Suc. filled memory for Thread #" + id + "\n";
  std::string log_fail = " > Failed to fill memory for Thread #" + id + "\n";

  std::string wrk_alloc = " Alloc. mem\n Thread #" + id + "\n\n";
  std::string wrk_suc = " Success filled\n memory Thread #" + id + "\n\n";
  std::string wrk_fail = " Failed to fill\n memory Thread #" + id + "\n\n";

  // Number to iterate to
  int tracker = 40 + rand() % 121;

  do {
    mcb->tcb_sema->wait(tcb);
    while (tcb->task_state == RUNNING) {
      if (counter == tracker) break;

      // Allocate and write a message to memory
      if (counter == tracker / 2) {

        // Write thread information
        mcb->ui->write_refresh(LOG_WINDOW, log_alloc);
        mcb->ui->write_refresh(args->id, wrk_alloc);

        // Allocate memory
        success = mcb->mem_man->allocate(msg.length());

        if (success) {
          mcb->ui->write_refresh(LOG_WINDOW, log_suc);
          mcb->ui->write_refresh(args->id, wrk_suc);
          mcb->mem_man->write(success, msg);
        } else {
          mcb->ui->write_refresh(LOG_WINDOW, log_fail);
          mcb->ui->write_refresh(args->id, wrk_fail);
        }
      }

      // Write to the task's window its current iteration
      mcb->ui->write_refresh(args->id, " Running #" + std::to_string(++counter) + "\n");
      usleep(150000);
    }

    // Release semaphore lock
    mcb->tcb_sema->signal();
    usleep(150000);
  } while (counter != tracker);

  // Release memory
  mcb->mem_man->free(success);

  // Signal end of thread
  mcb->ui->write_refresh(args->id, "\n Thread #" + id + "\n has ended.\n");
  mcb->ui->write_refresh(LOG_WINDOW, " Thread #" + id + " has ended.\n");

  // Set thread's state to DEAD
  mcb->scheduler->set_state(tcb, DEAD);
  return NULL;
}

/*
 * Ultima::unit_test_ipc(void*)
 * Test function for IPC.
 */
void* unit_test_ipc(void* arguments) {
  ARGUMENTS* args = (ARGUMENTS*)arguments;
  TASK_CONTROL_BLOCK* tcb = args->task_control_block;
  int& counter = args->thread_results;
  int success = 0;
  std::string id = std::to_string(args->id);
  MESSAGE_LIST* message_list = new MESSAGE_LIST;

  // Generate a random message
  std::string msg = message_list->message_lists[rand() % 20];

  // Information strings
  std::string log_fail = " > Failed to send message  Thread #" + id + "\n";
  std::string wrk_fail = " Failed to\n send message\n\n";

  // Number to iterate to
  int tracker = 40 + rand() % 121;

  do {
    mcb->tcb_sema->wait(tcb);
    while (tcb->task_state == RUNNING) {
      if (counter == tracker) break;

      // Send a message to another thread
      if (counter == tracker / 4) {
        int thread = 1 + rand() % 8;

        std::string wrk_suc = " > Msg sent:\n Thread #" + std::to_string(args->id) +
          "\n ->\n Thread #" + std::to_string(thread) + "\n\n";
        std::string log_suc = " > Msg sent: Thread #" + std::to_string(args->id) +
          " -> Thread #" + std::to_string(thread) + "\n";

        int success = mcb->ipc->message_send(mcb->ipc->compose_message(tcb, thread, msg));
        if (success == 1) {
          mcb->ui->write_refresh(LOG_WINDOW, log_suc);
          mcb->ui->write_refresh(args->id, wrk_suc);
        } else {
          mcb->ui->write_refresh(LOG_WINDOW, log_fail);
          mcb->ui->write_refresh(args->id, wrk_fail);
        }
      }

      // Check for new messages
      if (counter == tracker / 2) {
        std::string message = "";
        int success = mcb->ipc->message_receive(args->id, message);
        if (success == 1) {
          mcb->ui->write_refresh(LOG_WINDOW,
            " > Success received msg. from Thread #" + std::to_string(args->id));
          mcb->ui->write_refresh(args->id, " Success received msg.\n\n");
          mcb->ui->write_refresh(args->id, " " + message + "\n\n");
        } else {
          mcb->ui->write_refresh(LOG_WINDOW,
            " > Failed received msg. from Thread #" + std::to_string(args->id));
          mcb->ui->write_refresh(args->id, " Failed\n received msg.\n\n");
        }
      }

      // Write to the task's window its current iteration
      mcb->ui->write_refresh(args->id, " Running #" + std::to_string(++counter) + "\n");
      usleep(150000);
    }

    // Release semaphore lock
    mcb->tcb_sema->signal();
    usleep(150000);
  } while (counter != tracker);

  // Release memory
  mcb->mem_man->free(success);

  // Signal end of thread
  mcb->ui->write_refresh(args->id, "\n Thread #" + id + "\n has ended.\n");
  mcb->ui->write_refresh(LOG_WINDOW, " Thread #" + id + " has ended.\n");

  // Set thread's state to DEAD
  mcb->scheduler->set_state(tcb, DEAD);
  return NULL;
}