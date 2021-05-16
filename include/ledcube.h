#ifndef __LEDCUBE_H_
#define __LEDCUBE_H_

// C library headers
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#define BUFFERSIZE 32
typedef struct { uint8_t buf[(BUFFERSIZE+1)]; } serial_data_t;

typedef enum {on = 1, off = 0} cube_led_t;

#define SIDE 4
typedef struct { cube_led_t array[SIDE]; } cube_row_t;

typedef struct { uint8_t cube[SIDE][SIDE]; } cube_t;

// Define the serial port file name: /dev/ttyACM0 for instance
void serialSetPortName(const char *);

// Open th file handle and set all the serial interface configuration
// parameters
bool serialOpen();

// Receive bytes from the serial line
ssize_t serialRead(uint8_t *, size_t);

// Send bytes to the serial line
ssize_t serialWrite(uint8_t *, size_t);

// Free serial session to the 4x4 led cube
void serialClose();

// Get one single character from keyboard
int cubeGetch();

// Initialize serial session to the 4x4 led cube
bool cubeInit();

// Turn all the leds off
void cubeClear();

// Turn all the leds on
void cubeSet();

// Turn on/off the 4 leds of one row
void cubeSetRow(uint8_t, uint8_t, cube_led_t, cube_led_t, cube_led_t, cube_led_t);

// Turn on/off one single led per row
void cubeSetLed(uint8_t, uint8_t, uint8_t, cube_led_t);

#endif // __LEDCUBE_H__
