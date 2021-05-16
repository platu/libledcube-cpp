#include <iostream>
#include <iomanip>
#include <limits>
#include <chrono>
#include <thread>
#include <unistd.h>

#include <ledcube.h>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

int main(int argc, char **argv) {
	uint8_t stage, rank;
	int speed = 100, cycle = 30, opt, loop;
	steady_clock::time_point begin, end;

	// command line arguments: -n 30 -s 100 for instance
	while ((opt = getopt(argc, argv, "n:s:")) != -1) {
		switch(opt) {
			case 'n':
				cycle = atoi(optarg);
				break;
			case 's':
				speed = atoi(optarg);
				break;
			default:
				cerr << "Usage: " << argv[0] << " [-n] number of cycles [-s] speed in ms" << endl;;
		}
	}

	if (cubeInit()) {
		// Clear all leds
		cubeClear();

		begin = steady_clock::now();
		for (loop = cycle; loop >= 0; loop--) {
			for (stage = 0; stage < 4; stage++)
				for (rank = 0; rank < 4; rank++) {
					cubeSetRow(stage, rank, on, on, on, on);
					sleep_until(system_clock::now() + milliseconds(speed));
					cubeSetRow(stage, rank, off, off, off, off);
				}
			if ((loop < cycle) && (loop % 10 == 0)) {
				end = steady_clock::now();
				cout << "Elapsed time = " << duration_cast<milliseconds>(end - begin).count() << " ms" << endl;
				begin = steady_clock::now();
			}
		}

		cubeClear();
		serialClose();
	}

	return(EXIT_SUCCESS);
}
