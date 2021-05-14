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

void serialSetPortName(const char *);

bool serialOpen();

ssize_t serialRead(uint8_t *, size_t);

ssize_t serialWrite(uint8_t *, size_t);

void serialClose();

int cubeGetch();

bool cubeInit();

void cubeClear();

void cubeSet();

void cubeSetRow(uint8_t, uint8_t, cube_led_t, cube_led_t, cube_led_t, cube_led_t);

void cubeSetLed(uint8_t, uint8_t, uint8_t, cube_led_t);

#endif // __LEDCUBE_H__
