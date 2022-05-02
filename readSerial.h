#include <stdio.h>
#include <string>
// Windows header
#include <windows.h>

class FileCreationFailedException : public std::exception
{
    virtual const char *what() const noexcept
    {
        return "File could not be created.";
    }
};

class FlushFailedException : public std::exception
{
    virtual const char *what() const noexcept
    {
        return "Flush failed.";
    }
};

class SetTimeoutsFailedException : public std::exception
{
    virtual const char *what() const noexcept
    {
        return "Setting timeouts failed.";
    }
};

class ConfigurationFailedException : public std::exception
{
    virtual const char *what() const noexcept
    {
        return "Configruation failed..";
    }
};

class SerialReader
{
public:
    HANDLE port;
    uint8_t read_buf[512];
    SerialReader(std::string device_path)
    { // device_path is the string associated with the com port (eg. "COM4")
        port = CreateFileA(
            device_path.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if (port == INVALID_HANDLE_VALUE)
        {
            throw FileCreationFailedException();
        }

        // Flush away any bytes previously read or written.
        BOOL success = FlushFileBuffers(port);
        if (!success)
        {
            throw FlushFailedException();
        }

        // Configure read and write operations to time out after 100 ms.
        COMMTIMEOUTS timeouts = {0};
        timeouts.ReadIntervalTimeout = 0;
        timeouts.ReadTotalTimeoutConstant = 100;
        timeouts.ReadTotalTimeoutMultiplier = 0;
        timeouts.WriteTotalTimeoutConstant = 100;
        timeouts.WriteTotalTimeoutMultiplier = 0;

        success = SetCommTimeouts(port, &timeouts);
        if (!success)
        {
            throw SetTimeoutsFailedException();
        }
        
        // Set the baud rate and other options.
        DCB state = {0};
        state.DCBlength = sizeof(DCB);
        state.BaudRate = 9600;
        state.ByteSize = 8;
        state.Parity = NOPARITY;
        state.StopBits = ONESTOPBIT;
        
        success = SetCommState(port, &state);
        if (!success)
        {
            throw ConfigurationFailedException();
        }
    }

    auto read_serial() -> void;
    auto clear_buf() -> void;

    ~SerialReader()
    {
        CloseHandle(port);
    }
};
