#pragma once

#define ARDUINO_WAIT_TIME 2000

#if _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <getopt.h>
#endif

#include <iostream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>

#define WriteBufSize 256

class Serial
{
private:
#if _WIN32
    //Serial comm handler
    HANDLE hSerial;
    //Get various information about the connection
    COMSTAT status;
    //Keep track of last error
    DWORD errors;
#else
    //FILE* serialFile;
    int file_descriptor;
#endif

    //Connection status
    bool connected;

public:
    //Initialize Serial communication with the given COM port
    Serial(const char* portName);
    //Close the connection
    ~Serial();
    //Read data in a buffer, if nbChar is greater than the
    //maximum number of bytes available, it will return only the
    //bytes available. The function return -1 when nothing could
    //be read, the number of bytes actually read.
    int ReadData(char* buffer, unsigned int nbChar);
    //Writes data from a buffer through the Serial connection
    //return true on success.
    bool WriteData(const char* buffer, unsigned int nbChar);
    //Check if we are actually connected
    bool WriteString(const char* string);
    bool WriteFormat(const char* format, ...);
    bool IsConnected();
};
