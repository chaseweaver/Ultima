#include "../inc/UI.h"

WINDOW* UI::fetch_window(int window_id) {
  WINDOW_OBJECT* win_obj;
  bool success = false;

  while (!success && !window_object->empty()) {
    window_object->dequeue(win_obj);

    if (win_obj->window_id == window_id) {
      window_object->enqueue(win_obj);
      return win_obj->window;
    }

    window_object->enqueue(win_obj);
  }

  return NULL;
}

/*
 * UI::write_window(WINDOW*, std::string)
 * Writes message to window. DOES NOT REFRESH.
 */
bool UI::write_window(WINDOW* win, std::string msg) {
  int len = msg.length();
  char char_array[len + 1];
  strcpy(char_array, msg.c_str());

  wprintw(win, char_array);
  return true;
}

/*
 * UI::write_window(WINDOW*, int, int std::string)
 * Writes message to window at (X, Y). DOES NOT REFRESH.
 */
bool UI::write_window(WINDOW* win, int x, int y, std::string msg) {
  int len = msg.length();
  char char_array[len + 1];
  strcpy(char_array, msg.c_str());

  mvwprintw(win, y, x, char_array);
  return true;
}

/*
 * UI::write_window_refresh(WINDOW*, std::string)
 * Writes message to window. REFRESHES
 */
bool UI::write_window_refresh(WINDOW* win, std::string msg) {
  int len = msg.length();
  char char_array[len + 1];
  strcpy(char_array, msg.c_str());

  wprintw(win, char_array);
  wrefresh(win);
  return true;
}

/*
 * UI::write_window_refresh(WINDOW*, int, int std::string)
 * Writes message to window at (X, Y). REFRESHES
 */
bool UI::write_window_refresh(WINDOW* win, int x, int y, std::string msg) {
  int len = msg.length();
  char char_array[len + 1];
  strcpy(char_array, msg.c_str());

  mvwprintw(win, y, x, char_array);
  wrefresh(win);
  return true;
}

/*
 * UI::UI()
 * Default constructor.
 */
UI::UI(MASTER_CONTROL_BLOCK* mcb) : mcb(mcb) {
  window_object = new Queue< WINDOW_OBJECT* >;
  initscr();
}

/*
 * UI::~UI()
 * Default deconstructor.
 */
UI::~UI() { endwin(); }

/*
 * UI::create_window_spawn(std::string, int, int, int, int, int, int, int)
 * Adds a Curses window with a title (X, Y) to the UI window object list.
 * Returns a pointer to the WINDOW object.
 */
WINDOW* UI::create_window_spawn(std::string title,
  int title_x,
  int title_y,
  int window_id,
  int width,
  int height,
  int x,
  int y) {

  WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
  WINDOW* win = newwin(height, width, y, x);
  scrollok(win, true);
  scroll(win);
  box(win, 0, 0);

  win_obj->window_id = window_id;
  win_obj->window_width = width;
  win_obj->window_height = height;
  win_obj->window_x = x;
  win_obj->window_y = y;
  win_obj->window = win;

  mcb->ui_sema->wait();

  window_object->enqueue(win_obj);
  write_window(win, title_x, title_y, title);
  wrefresh(win);

  mcb->ui_sema->signal();
  return win;
}

/*
 * UI::create_window(std::string, int, int, int, int, int, int, int)
 * Adds a Curses window with a title (X, Y) to the UI window object list.
 * Returns a pointer to the WINDOW object.
 */
WINDOW* UI::create_window(std::string title,
  int title_x,
  int title_y,
  int window_id,
  int width,
  int height,
  int x,
  int y) {

  WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
  WINDOW* win = newwin(height, width, y, x);
  scrollok(win, true);
  scroll(win);
  box(win, 0, 0);

  win_obj->window_id = window_id;
  win_obj->window_width = width;
  win_obj->window_height = height;
  win_obj->window_x = x;
  win_obj->window_y = y;
  win_obj->window = win;

  mcb->ui_sema->wait();

  window_object->enqueue(win_obj);
  write_window(win, title_x, title_y, title);

  mcb->ui_sema->signal();
  return win;
}

/*
 * UI::create_window_spawn(std::string, int, int, int, int, int)
 * Adds a Curses window with a title to the UI window object list and SPAWNS it.
 * Returns a pointer to the WINDOW object.
 */
WINDOW* UI::create_window_spawn(std::string title,
  int window_id,
  int width,
  int height,
  int x,
  int y) {
  WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
  WINDOW* win = newwin(height, width, y, x);
  WINDOW* write_win = newwin(height - 4, width - 3, y + 3, x + 2);
  scrollok(win, true);
  scroll(win);
  scrollok(write_win, true);
  scroll(write_win);
  box(win, 0, 0);

  win_obj->window_id = window_id;
  win_obj->window_width = width;
  win_obj->window_height = height;
  win_obj->window_x = x;
  win_obj->window_y = y;
  win_obj->window = write_win;

  mcb->ui_sema->wait();

  window_object->enqueue(win_obj);
  int offset_x = (width / 2) - (title.length() / 2);
  write_window(win, offset_x, 1, title);
  wrefresh(win);
  wrefresh(write_win);

  mcb->ui_sema->signal();
  return write_win;
}

/*
 * UI::create_window(std::string, int, int, int, int, int)
 * Adds a Curses window with a title to the UI window object list.
 * Returns a pointer to the WINDOW object.
 */
WINDOW* UI::create_window(std::string title,
  int window_id,
  int width,
  int height,
  int x,
  int y) {
  WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
  WINDOW* win = newwin(height, width, y, x);
  WINDOW* write_win = newwin(height - 4, width - 3, y + 3, x + 2);
  scrollok(win, true);
  scroll(win);
  scrollok(write_win, true);
  scroll(write_win);
  box(win, 0, 0);
  box(write_win, 0, 0);

  win_obj->window_id = window_id;
  win_obj->window_width = width;
  win_obj->window_height = height;
  win_obj->window_x = x;
  win_obj->window_y = y;
  win_obj->window = write_win;

  mcb->ui_sema->wait();

  window_object->enqueue(win_obj);
  int offset_x = (width / 2) - (title.length() / 2);
  write_window(win, offset_x, 1, title);

  mcb->ui_sema->signal();
  return write_win;
}

/*
 * UI::create_window_spawn(int, int, int, int, int)
 * Adds a Curses window to the UI window object list and SPAWNS it.
 * Returns a pointer to the WINDOW object.
 */
WINDOW* UI::create_window_spawn(int window_id, int width, int height, int x, int y) {
  WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
  WINDOW* win = newwin(height, width, y, x);
  scrollok(win, TRUE);
  scroll(win);
  box(win, 0, 0);

  win_obj->window_id = window_id;
  win_obj->window_width = width;
  win_obj->window_height = height;
  win_obj->window_x = x;
  win_obj->window_y = y;
  win_obj->window = win;

  mcb->ui_sema->wait();

  window_object->enqueue(win_obj);
  wrefresh(win);

  mcb->ui_sema->signal();
  return win;
}

/*
 * UI::create_window(int, int, int, int, int)
 * Adds a Curses window to the UI window object list.
 * Returns a pointer to the WINDOW object.
 */
WINDOW* UI::create_window(int window_id, int width, int height, int x, int y) {
  WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
  WINDOW* win = newwin(height, width, y, x);
  scrollok(win, TRUE);
  scroll(win);
  box(win, 0, 0);

  win_obj->window_id = window_id;
  win_obj->window_width = width;
  win_obj->window_height = height;
  win_obj->window_x = x;
  win_obj->window_y = y;
  win_obj->window = win;

  mcb->ui_sema->wait();

  window_object->enqueue(win_obj);

  mcb->ui_sema->signal();
  return win;
}

/*
 * UI::create_window(std::string, int, int, int, int, int, int, int)
 * Adds a Curses window with a title (X, Y) to the UI window object list and SPAWNS
 * it. Locks window within another. Returns a pointer to the WINDOW object.
 */
WINDOW* UI::create_window_lock_spawn(std::string title,
  int title_x,
  int title_y,
  int window_id,
  int width,
  int height,
  int x,
  int y) {

  WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
  WINDOW* win = newwin(height, width, y, x);
  WINDOW* write_win = newwin(height - 2, width - 2, y + 1, x + 1);
  scrollok(win, true);
  scroll(win);
  scrollok(write_win, true);
  scroll(write_win);
  box(win, 0, 0);

  win_obj->window_id = window_id;
  win_obj->window_width = width;
  win_obj->window_height = height;
  win_obj->window_x = x;
  win_obj->window_y = y;
  win_obj->window = write_win;

  mcb->ui_sema->wait();

  window_object->enqueue(win_obj);
  write_window(win, title_x, title_y, title);
  wrefresh(win);
  wrefresh(write_win);

  mcb->ui_sema->signal();
  return write_win;
}

/*
 * UI::create_window(std::string, int, int, int, int, int, int, int)
 * Adds a Curses window with a title (X, Y) to the UI window object list.
 * Locks window within another.
 * Returns a pointer to the WINDOW object.
 */
WINDOW* UI::create_window_lock(std::string title,
  int title_x,
  int title_y,
  int window_id,
  int width,
  int height,
  int x,
  int y) {

  WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
  WINDOW* win = newwin(height, width, y, x);
  WINDOW* write_win = newwin(height - 2, width - 2, y + 1, x + 1);
  scrollok(win, true);
  scroll(win);
  scrollok(write_win, true);
  scroll(write_win);
  box(win, 0, 0);

  win_obj->window_id = window_id;
  win_obj->window_width = width;
  win_obj->window_height = height;
  win_obj->window_x = x;
  win_obj->window_y = y;
  win_obj->window = write_win;

  mcb->ui_sema->wait();

  window_object->enqueue(win_obj);
  write_window(win, title_x, title_y, title);

  mcb->ui_sema->signal();
  return write_win;
}

/*
 * UI::write(int, int, int, std::string, TASK_CONTROL_BLOCK*)
 * Writes a message to a window given ID and (X, Y) and stores TASK ID.
 */
void UI::write(int window_id, int x, int y, std::string msg, TASK_CONTROL_BLOCK* tcb) {
  mcb->tcb_sema->wait(tcb);

  WINDOW* win = fetch_window(window_id);
  write_window(win, x, y, msg);

  mcb->tcb_sema->signal();
}

/*
 * UI::write(int, int, int, std::string)
 * Writes a message to a window given ID and (X, Y).
 */
void UI::write(int window_id, int x, int y, std::string msg) {
  mcb->ui_sema->wait();

  WINDOW* win = fetch_window(window_id);
  write_window(win, x, y, msg);

  mcb->ui_sema->signal();
}

/*
 * UI::write(int, std::string, TASK_CONTROL_BLOCK*)
 * Writes a message to a window given ID and stores TASK CONTROL BLOCK.
 */
void UI::write(int window_id, std::string msg, TASK_CONTROL_BLOCK* tcb) {
  mcb->tcb_sema->wait(tcb);

  WINDOW* win = fetch_window(window_id);
  write_window(win, msg);

  mcb->tcb_sema->signal();
}

/*
 * UI::write(int, std::string)
 * Writes a message to a window given ID.
 */
void UI::write(int window_id, std::string msg) {
  mcb->ui_sema->wait();

  WINDOW* win = fetch_window(window_id);
  write_window(win, msg);

  mcb->ui_sema->signal();
}

/*
 * UI::write(int, int, int, std::string, TASK_CONTROL_BLOCK*)
 * Writes a message to a window given ID and (X, Y) and stores TASK ID.
 */
void UI::write_refresh(int window_id,
  int x,
  int y,
  std::string msg,
  TASK_CONTROL_BLOCK* tcb) {
  mcb->tcb_sema->wait(tcb);

  WINDOW* win = fetch_window(window_id);
  write_window_refresh(win, x, y, msg);

  mcb->tcb_sema->signal();
}

/*
 * UI::write(int, int, int, std::string)
 * Writes a message to a window given ID and (X, Y).
 */
void UI::write_refresh(int window_id, int x, int y, std::string msg) {
  mcb->ui_sema->wait();

  WINDOW* win = fetch_window(window_id);
  write_window_refresh(win, x, y, msg);

  mcb->ui_sema->signal();
}

/*
 * UI::write(int, std::string, TASK_CONTROL_BLOCK*)
 * Writes a message to a window given ID and stores TASK CONTROL BLOCK.
 */
void UI::write_refresh(int window_id, std::string msg, TASK_CONTROL_BLOCK* tcb) {
  mcb->tcb_sema->wait(tcb);

  WINDOW* win = fetch_window(window_id);
  write_window_refresh(win, msg);

  mcb->tcb_sema->signal();
}

/*
 * UI::write(int, std::string)
 * Writes a message to a window given ID.
 */
void UI::write_refresh(int window_id, std::string msg) {
  mcb->ui_sema->wait();

  WINDOW* win = fetch_window(window_id);
  write_window_refresh(win, msg);

  mcb->ui_sema->signal();
}

/*
 * UI::clear_window(WINDOW*)
 * Clears the window.
 */
void UI::clear_window(WINDOW* win) {
  wclear(win);
  wrefresh(win);
}

/*
 * UI::clear_window(int)
 * Clears the window.
 */
void UI::clear_window(int window_id) {
  mcb->ui_sema->wait();
  Queue< WINDOW_OBJECT* >* win_obj = new Queue< WINDOW_OBJECT* >(*window_object);
  mcb->ui_sema->signal();

  bool success = false;
  do {
    WINDOW_OBJECT* tmp;

    mcb->ui_sema->wait();

    win_obj->dequeue(tmp);
    if (tmp->window_id == window_id) {
      wclear(tmp->window);
      wrefresh(tmp->window);
      success = true;
    }

    mcb->ui_sema->signal();
  } while (!win_obj->empty() || !success);
}

/*
 * UI::get_window_amount()
 * Returns amount of created windows.
 */
int UI::get_window_amount() { return window_object->size(); }