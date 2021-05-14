#include <iostream>
#include <iomanip>
#include <limits>

#include <ledcube.h>

using namespace std;

int main() {
	uint8_t stage, rank, row;

	if (cubeInit()) {
		// Clear all leds
		cubeClear();

		// Set one row of 4 leds once
		cout << "Send on off on off to one row." << endl;
		cout << "Choose stage [0..3] and rank [0..3] of the cube: ";
		cin >> stage >> rank;
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		cubeSetRow(stage, rank, on, off, on, off);

		// Set one single led
		cout << "Send on to a single led." << endl;
		cout << "Choose stage [0..3], rank [0..3], and row [0..3] of the cube: ";
		cin >> stage >> rank >> row;
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		cubeSetLed(stage, rank, row, on);

		cout << "Waiting for keypress." << endl;
		cubeGetch();

		// Turn on all the leds
		cubeSet();

		cout << "-------------------------------" << endl
			<< "Waiting for keypress." << endl;
		cubeGetch();
		cubeClear();
		serialClose();
	}

	return(EXIT_SUCCESS);
}
