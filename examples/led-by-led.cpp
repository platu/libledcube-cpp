#include <iostream>
#include <iomanip>
#include <limits>
#include <chrono>
#include <thread>

#include <ledcube.h>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

#define SIDE 4

int main() {
	uint8_t stage, rank, row;
	steady_clock::time_point begin, end;

	if (cubeInit()) {
		// Clear all leds
		cubeClear();

		// Turn all the leds on one by one
		for (stage = 0; stage < SIDE; stage++)
			for (rank = 0; rank < SIDE; rank++)
				for (row = 0; row < SIDE; row++) {
					cout << "stage: " << (uint16_t)stage
						<< " rank: " << (uint16_t)rank
						<< " row: " << (uint16_t)row << endl;
					cubeSetLed(stage, rank, row, on);
					sleep_until(system_clock::now() + milliseconds(50));
				}

		cout << "-------------------------------" << endl
			<< "Waiting for keypress." << endl;
		cubeGetch();

		// Turn all the leds off one by one
		for (stage = 0; stage < SIDE; stage++)
			for (rank = 0; rank < SIDE; rank++)
				for (row = 0; row < SIDE; row++) {
					cout << "stage: " << (uint16_t)stage
						<< " rank: " << (uint16_t)rank
						<< " row: " << (uint16_t)row << endl;
					cubeSetLed(stage, rank, row, off);
					sleep_until(system_clock::now() + milliseconds(50));
				}

		serialClose();
	}

	return(EXIT_SUCCESS);
}
