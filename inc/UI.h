#ifndef UI_H
#define UI_H

#ifdef _MSC_VER
#pragma once
#endif 

#include "LinkedList.h"
#include <ncurses.h>

class UI {
private:
	int refresh_rate = 60;
	LinkedList<WINDOW*> window_list;
};



#endif