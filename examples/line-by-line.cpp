#include <iostream>
#include <iomanip>
#include <limits>
#include <chrono>
#include <thread>

#include <ledcube.h>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

#define LOOP_MAX 100

int main() {
	uint8_t stage, rank;
	int loop;
	steady_clock::time_point begin, end;

	if (cubeInit()) {
		// Clear all leds
		cubeClear();

		begin = steady_clock::now();
		for (loop = LOOP_MAX; loop > 0; loop--) {
			for (stage = 0; stage < 4; stage++)
				for (rank = 0; rank < 4; rank++) {
					cubeSetRow(stage, rank, on, on, on, on);
					sleep_until(system_clock::now() + milliseconds(30));
					cubeSetRow(stage, rank, off, off, off, off);
				}
		}
		end = steady_clock::now();
		cout << "Elapsed time = " << duration_cast<milliseconds>(end - begin).count() << " ms" << endl;

		cout << "-------------------------------" << endl
			<< "Waiting for keypress." << endl;
		cubeGetch();
		cubeClear();
		serialClose();
	}

	return(EXIT_SUCCESS);
}
