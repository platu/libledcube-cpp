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
	int speed = 400, cycle = 3, opt, drop_sp;

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

		cout << "Turn on upper stage then drop to the ground: "
			<< cycle << " cycles with a start delay of "
			<< speed << "ms." << endl;
		while(cycle > 0) {
			drop_sp = speed; // ms
			for (stage = 0; stage < 4; stage++) {
				// Turn on
				for (rank = 0; rank < 4; rank++)
					cubeSetRow(3 - stage, rank, on, on, on, on);

				// Delay decreases during the fall
				sleep_until(system_clock::now() + milliseconds(drop_sp / (stage + 1)));

				// Turn off
				for (rank = 0; rank < 4; rank++)
					cubeSetRow(3 - stage, rank, off, off, off, off);
			}
			cycle--;
		}

		serialClose();
	}

	return(EXIT_SUCCESS);
}
