#include "Serial.h"

Serial::Serial(const char* portName)
{
    //We're not yet connected
    this->connected = false;

#if _WIN32
    //Try to connect to the given port throuh CreateFile
    this->hSerial = CreateFileA(portName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    //Check if the connection was successfull
    if (this->hSerial == INVALID_HANDLE_VALUE)
    {
        //If not success full display an Error
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {

            //Print Error if neccessary
            fprintf(stderr, "ERROR: Handle was not attached. Reason: %s not available.\n", portName);

        }
        else
        {
            fprintf(stderr, "ERROR!!!");
        }
    }
    else
    {
        //If connected we try to set the comm parameters
        DCB dcbSerialParams = { 0 };

        //Try to get the current
        if (!GetCommState(this->hSerial, &dcbSerialParams))
        {
            //If impossible, show an error
            fprintf(stderr, "failed to get current serial parameters!");
        }
        else
        {
            //Define serial connection parameters for the arduino board
            dcbSerialParams.BaudRate = CBR_9600;
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity = NOPARITY;
            //Setting the DTR to Control_Enable ensures that the Arduino is properly
            //reset upon establishing a connection
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

            //Set the parameters and check for their proper application
            if (!SetCommState(hSerial, &dcbSerialParams))
            {
                fprintf(stderr, "ALERT: Could not set Serial Port parameters");
            }
            else
            {
                //If everything went fine we're connected
                this->connected = true;
                //Flush any remaining characters in the buffers 
                PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
                //We wait 2s as the arduino board will be reseting
                Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }
#else
    // Standard struct for holding port data/settings
    struct termios toptions;

    // Read/write, no delays
    int fd = open(portName, O_RDWR | O_NOCTTY); // | O_NDELAY);
    if (fd == -1)
    {
        perror("Serial: Unable to open port ");
    }
    else
    {
        if (tcgetattr(fd, &toptions) < 0)
        {
            perror("Serial: Couldn't get term attributes");
        }
        else
        {
            speed_t brate = B9600;

            cfsetispeed(&toptions, brate);
            cfsetospeed(&toptions, brate);

            // 8N1, no hw flow control, configured for read access as well as write
            toptions.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS | CREAD | CLOCAL);
            toptions.c_cflag |= CS8;

            // turn on s/w flow ctrl
            toptions.c_iflag |= (IXON | IXOFF | IXANY);
            // make sure the CR and NL aren't mapped to each other on input
            toptions.c_iflag &= ~(INLCR | ICRNL);
            // THE ABOVE OPTION'S EXISTENCE IS NECESSARY. IF YOU REMOVE IT
            //  YOU WILL SPEND HOURS PULLING YOUR HAIR OUR TROUBLESHOOTING
            //  STRINGS THAT JUST WON'T PARSE BECAUSE '\r' IS REPLACED WITH '\n'
            //  by the cheap serial->USB adapter you have or the different
            //  default port settings of your kernel. This bit me.

            // make raw output as well
            toptions.c_oflag &= ~OPOST;
            // make sure the CR and NL aren't mapped to each other on output
            toptions.c_oflag &= ~(INLCR | ICRNL);
            // make raw input, not line-based canonical
            toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
            // Not necessary, but peace of mind to set timeouts
            toptions.c_cc[VMIN] = 0;
            toptions.c_cc[VTIME] = 2;

            if (tcsetattr(fd, TCSAFLUSH, &toptions) < 0)
            {
                // Self explanatory, attempts to set the port attribs
                perror("Serial: Couldn't set term attributes");
            }
            else
            {
                this->file_descriptor = fd;

                this->connected = true;

                // wait until we're done initializing o do anything else.
                sleep(1);
            }
        }
    }
#endif
}

Serial::~Serial()
{
    //Check if we are connected before trying to disconnect
    if (this->connected)
    {
        //We're no longer connected
        this->connected = false;

        //Close the serial handler
#if _WIN32
        CloseHandle(this->hSerial);
#else
        //fclose(serialFile);

        close(this->file_descriptor);
#endif
    }
}

int Serial::ReadData(char* buffer, unsigned int nbChar)
{
#if _WIN32
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(this->hSerial, &this->errors, &this->status);

    //Check if there is something to read
    if (this->status.cbInQue > 0)
    {
        //If there is we check if there is enough data to read the required number
        //of characters, if not we'll read only the available characters to prevent
        //locking of the application.
        if (this->status.cbInQue > nbChar)
        {
            toRead = nbChar;
        }
        else
        {
            toRead = this->status.cbInQue;
        }

        //Try to read the require number of chars, and return the number of read bytes on success
        if (ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL))
        {
            return bytesRead;
        }

    }
#else
    bzero(buffer, nbChar);
    size_t bytes_read = read(this->file_descriptor, buffer, nbChar);
    buffer[bytes_read] = '\0';
    return bytes_read;
#endif

    //If nothing has been read, or that an error was detected return 0
    return 0;
}

bool Serial::WriteData(const char* buffer, unsigned int nbChar)
{
#if _WIN32
    DWORD bytesSend;

    //Try to write the buffer on the Serial port
    if (!WriteFile(this->hSerial, (void*)buffer, nbChar, &bytesSend, 0))
    {
        //In case it don't work get comm error and return false
        ClearCommError(this->hSerial, &this->errors, &this->status);

        return false;
    }
    else
        return true;
#else
    //int bytesWritten = fwrite(buffer, 1, nbChar, serialFile);

    int bytesWritten;

    bytesWritten = write(this->file_descriptor, buffer, nbChar);

    //fprintf(stderr, "To write: %d, Written: %d\n", nbChar, bytesWritten);

    if (bytesWritten == -1)
        perror("Write error: ");

    return (bytesWritten == (int)nbChar);
#endif
}

bool Serial::WriteString(const char* string)
{
    //fprintf(stderr, "** Serial write: %s\n", string);

    return WriteData(string, (unsigned int)strlen(string));
}

bool Serial::WriteFormat(const char* format, ...)
{
    char buf[256];

    va_list args;
    va_start(args, format);
    vsnprintf(buf, WriteBufSize, format, args);
    va_end(args);

    return WriteString(buf);
}

bool Serial::IsConnected()
{
    //Simply return the connection status
    return this->connected;
}