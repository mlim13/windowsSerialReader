#include <stdio.h>
#include <iostream>
// Windows header
#include <windows.h>

#include "readSerial.h"

auto SerialReader::read_serial() -> void
{
    DWORD received;
    BOOL success = ReadFile(port, read_buf, sizeof(read_buf), &received, NULL);
    if (!success)
    {
        std::cout << "Failed to read from port." << std::endl;
    }
}
auto SerialReader::clear_buf() -> void
{
    memset(read_buf, 0, sizeof(read_buf));
}