// C library headers
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <inttypes.h>
#include <cstdbool>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <assert.h>
#include <sys/ioctl.h>

#include "../include/ledcube.h"

#define FILENAMELENGTH 20
static char _portName[FILENAMELENGTH];
static int _serialHandle = -1;
static struct termios _tty;

void serialSetPortName(const char *name) {
	strcpy(_portName, name);
}

bool serialOpen() {
	bool retOk = true;

	_serialHandle = open(_portName, O_RDWR);
	// Check for errors
	if (_serialHandle < 0) {
		printf("Error %i from open: %s\n", errno, strerror(errno));
		retOk = false;
	}
	// Read in existing settings, and handle any error
	else if (tcgetattr(_serialHandle, &_tty) != 0) {
		printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
		retOk = false;
	}
	else {
		_tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
		_tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
		_tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
		_tty.c_cflag |= CS8; // 8 bits per byte (most common)
		_tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
		_tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

		_tty.c_lflag &= ~ICANON;
		_tty.c_lflag &= ~ECHO; // Disable echo
		_tty.c_lflag &= ~ECHOE; // Disable erasure
		_tty.c_lflag &= ~ECHONL; // Disable new-line echo
		_tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
		_tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off software flow ctrl
		_tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

		_tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
		_tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
		// _tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
		// _tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

		_tty.c_cc[VTIME] = 1;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
		_tty.c_cc[VMIN] = 0;

		// Set in/out baud rate to be 19200
		cfsetispeed(&_tty, B19200);
		cfsetospeed(&_tty, B19200);

		// Save tty settings, also checking for error
		if (tcsetattr(_serialHandle, TCSANOW, &_tty) != 0) {
			printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
			retOk = false;
		}

/*		int flag = TIOCM_DTR;
		// Clear DTR flag manually
		ioctl(_serialHandle,TIOCMBIC,&flag);
		//sleep(1);
		// Set DTR flag manually
		ioctl(_serialHandle,TIOCMSET,&flag);*/
	}

	return retOk;
}

// Reads bytes from the serial port.
// Returns the number of bytes successfully read into the buffer, or -1 if
// there was an error reading.
ssize_t serialRead(uint8_t * buffer, size_t size)
{
	ssize_t received = 0;

	while (received < (ssize_t)size) {
		ssize_t num_bytes = read(_serialHandle, buffer + received, size - received);

		if (num_bytes < 0) {
			printf("Error reading: %s", strerror(errno));
			received = -1;
		}
		else if (num_bytes == 0) {
			received = 0; // Timeout
		}
		else
			received += num_bytes;
	}

	return received;
}

ssize_t serialWrite(uint8_t * buffer, size_t size)
{
	ssize_t sent = 0;

	ssize_t num_bytes = write(_serialHandle, buffer, size);

	if (num_bytes != (ssize_t)size) {
		printf("Error reading: %s", strerror(errno));
		sent = -1;
	}

	return sent;
}

void serialClose() {
	close(_serialHandle);
}

int cubeGetch() {
	int c=0;

	struct termios org_opts, new_opts;
	int res=0;

	//----- store current settings -------------
	res=tcgetattr(STDIN_FILENO, &org_opts);
	assert(res==0);
	//----- set new terminal parameters --------
	memcpy(&new_opts, &org_opts, sizeof(new_opts));
	new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
	//------ wait for a single key -------------
	c=getchar();
	//------ restore current settings- ---------
	res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
	assert(res==0);

	return c;
}

bool cubeInit() {
	bool retOk = false;
	serial_data_t msg;
	int num_read;
	const char *prompt = "Raspberry ?";

	serialSetPortName("/dev/ttyACM0");

	if (serialOpen()) {

		puts ("First, reset the nucleo board");
		puts ("Second, hit any key");
		puts ("Waiting for keypress.");
		cubeGetch();

		memset(msg.buf, '\0', sizeof(msg));
		num_read = serialRead(msg.buf, 12);
		if (num_read > 0) {
			printf ("%s\n", msg.buf);
			if (strncmp((char *)msg.buf, prompt, strlen(prompt)) == 0) {
				serialWrite((uint8_t *)"y\n", 2);
				puts ("Cube is ready to receive lighting instructions");
				retOk = true;
			}
		}
	}
	return retOk;
}

void cubeClear() {
	uint8_t stage, rank, i;
	serial_data_t line;

	for (stage = 0; stage < 4; stage++)
		for (rank = 0; rank < 4; rank++) {
			i = stage * 4 + rank;
			line.buf[i] = (stage << 6 | rank << 4) & 0xf0;
		}
	serialWrite(line.buf, 16);
}

void cubeSet() {
	uint8_t stage, rank, i;
	serial_data_t line;

	for (stage = 0; stage < 4; stage++)
		for (rank = 0; rank < 4; rank++) {
			i = stage * 4 + rank;
			line.buf[i] = (stage << 6 | rank << 4) | 0x0f;
		}
	serialWrite(line.buf, 16);
}

void cubeSetRow(uint8_t stage, uint8_t rank, cube_led_t r0, cube_led_t r1, cube_led_t r2, cube_led_t r3) {
	serial_data_t line;

	stage &= 0x03;
	rank &= 0x03;

	line.buf[0] = stage << 6 | rank << 4 | r3 << 3 | r2 << 2 | r1 << 1 | r0;
	serialWrite(line.buf, 1);
}

void cubeSetLed(uint8_t stage, uint8_t rank, uint8_t row, cube_led_t status) {
	serial_data_t line;

	stage &= 0x03;
	rank &= 0x03;
	row &= 0x03;

	line.buf[0] = stage << 6 | rank << 4 | (uint8_t)status << row;
	serialWrite(line.buf, 1);
}
