#include "Engine.h"


/*
Project: Xenowars, Driver code
Created: 31 MAR 2026
Updated: 19 MAR 2026

Description:
	This file contains the driver code for the engine.

*/

int main(int argc, char** argv) {
	Engine engine;


	while (engine.getRunning()) {
		engine.poll();
		engine.update();
		engine.draw();
	}

	return 0;
}