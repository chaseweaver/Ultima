#include "UI.h"
#include <iostream>

int main() {
	
	UI ui;
	ui.add_window(1, 20, 20, 0, 0);

	ui.write(1, 1, 1, "One");

	ui.write(1, 1, 2, "Two");

	ui.start(ui);

	ui.write(1, 1, 3, "Three");

	return 0;
}