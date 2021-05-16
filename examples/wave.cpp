#include <iostream>
#include <iomanip>
#include <limits>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <cmath>

#include <ledcube.h>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

#define SIDE 4

int main(int argc, char **argv) {
	uint8_t x, y, z;
	int speed = 400, cycle = 3, cpt, opt;
	float phase = -1.5, scaled_x, scaled_y, scaled_z;

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
		cpt = cycle;
		while(cpt > 0) {
	        phase += 0.5;
			for (x = 0; x < SIDE; x++)
				for (y = 0; y < SIDE; y++) {
					scaled_x = x * 2 * M_PI / (SIDE - 1) - M_PI;
					scaled_y = y * 2 * M_PI / (SIDE - 1) - M_PI;
					scaled_z = sin(phase + sqrt(scaled_x * scaled_x + scaled_y * scaled_y));
					z = round((SIDE - 1)/2 * (scaled_z + 1));
					cubeSetLed(z, y, x, on);
					cout << fixed << setw(3);
					cout << "ON -> z: " << (uint16_t)z << " y: " << (uint16_t)y << " x: " << (uint16_t)x << endl;
					sleep_until(system_clock::now() + milliseconds(speed));
				}
			cpt--;
		}
		cpt = cycle;
		while(cpt > 0) {
	        phase -= 0.5;
			for (x = 0; x < SIDE; x++)
				for (y = 0; y < SIDE; y++) {
					scaled_x = x * 2 * M_PI / (SIDE - 1) - M_PI;
					scaled_y = y * 2 * M_PI / (SIDE - 1) - M_PI;
					scaled_z = sin(phase + sqrt(scaled_x * scaled_x + scaled_y * scaled_y));
					z = round((SIDE - 1)/2 * (scaled_z + 1));
					cubeSetLed(z, y, x, off);
					cout << fixed << setw(3);
					cout << "OFF -> z: " << (uint16_t)z << " y: " << (uint16_t)y << " x: " << (uint16_t)x << endl;
					sleep_until(system_clock::now() + milliseconds(speed));
				}
			cpt--;
		}

		cubeClear();
		serialClose();
	}

	return(EXIT_SUCCESS);
}
