#include "UI.h"
#include <iostream>

int main() {
	
	UI ui;
	ui.add_window(1, 20, 20, 0, 0);
	ui.start();
	ui.write(1, "penis");
	ui.write(1, "penis");
	ui.write(1, "penis");
	//std::cout << ui.get_window_amount() << std::endl;

	sleep(10);

	return 0;
}