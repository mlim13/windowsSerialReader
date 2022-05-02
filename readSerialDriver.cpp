// C library headers
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

#include "readSerial.h"

int main(int argc, char *argv[])
{
    if (argc != 3 or (std::string(argv[2]) != "1" and std::string(argv[2]) != "2" and std::string(argv[2]) != "3"))
    {
        std::cout << "Incorrect usage." << std::endl;
        std::cout << "Correct usage: ./readSerial COM<number> <mode>" << std::endl;
        std::cout << "  <number> = the number associated with the connected port" << std::endl;
        std::cout << "  <mode> = Either 1, 2, or 3 where" << std::endl;
        std::cout << "      1 = production mode - received commands are printed to the screen" << std::endl;
        std::cout << "      2 = debugging mode - received commands are printed to the screen in debug format" << std::endl;
        std::cout << "      3 = verbose mode - received commands and ranging data is printed to the screen" << std::endl;
        return 1;
    }
    auto arg2 = std::string(argv[2]); // Create C++ version of the string argument
    auto serial_reader = SerialReader(argv[1]);
    std::cout << "Ready to display data >" << std::endl;
    // Union defined to perform bit operations on float
    // Bit operations are used to avoid endianness issues with memcpy
    union {
        float f;
        uint32_t u;
    } fu = { .u = 0 };
    uint32_t temp = 0;
    int32_t steer;
    while (true)
    {
        serial_reader.read_serial();
        temp = 0; // reset temp to 0;
        if ((int)serial_reader.read_buf[0] == 0xE0)
        {
            if (arg2 == "1")
            {
                if (serial_reader.read_buf[16] == 0x90)
                {
                    assert(sizeof(float) == 4);
                    fu.u = 0;
                    temp = serial_reader.read_buf[17];
                    temp <<= 24;
                    fu.u |= temp;
                    temp = serial_reader.read_buf[18];
                    temp <<= 16;
                    fu.u |= temp;
                    temp = serial_reader.read_buf[19];
                    temp <<= 8;
                    fu.u |= temp;
                    temp = serial_reader.read_buf[20];
                    fu.u |= temp;
                    std::cout << "drive " << fu.f << std::endl;
                }
                else if (serial_reader.read_buf[16] == 0x91)
                {
                    steer = 0;
                    temp = serial_reader.read_buf[17];
                    temp <<= 24;
                    steer |= temp;
                    temp = serial_reader.read_buf[18];
                    temp <<= 16;
                    steer |= temp;
                    temp = serial_reader.read_buf[19];
                    temp <<= 8;
                    steer |= temp;
                    temp = serial_reader.read_buf[20];
                    steer |= temp;
                    
                    std::cout << "steer " << steer << std::endl;
                }
            }
            else
            {
                std::cout << "Encrypted: ";
                for (unsigned i = 0; i < 15; ++i)
                {
                    printf("%02x", serial_reader.read_buf[i] & 0xFF);
                    // std::cout << std::hex << (int)serial_reader.read_buf[i];
                }
                std::cout << " -> Decrypted: ";
                for (unsigned i = 15; i < 21; ++i)
                {
                    printf("%02x", serial_reader.read_buf[i] & 0xFF);
                    // std::cout << std::hex << (int)serial_reader.read_buf[i];
                }
                std::cout << std::endl;
            }
        }
        else if ((int)serial_reader.read_buf[0] == 0xD0)
        {
            if (arg2 == "1")
            {
                if (serial_reader.read_buf[1] == 0x90)
                {
                    assert(sizeof(float) == 4);
                    fu.u = 0;
                    temp = serial_reader.read_buf[2];
                    temp <<= 24;
                    fu.u |= temp;
                    temp = serial_reader.read_buf[3];
                    temp <<= 16;
                    fu.u |= temp;
                    temp = serial_reader.read_buf[4];
                    temp <<= 8;
                    fu.u |= temp;
                    temp = serial_reader.read_buf[5];
                    fu.u |= temp;
                    std::cout << "drive " << fu.f << std::endl;
                }
                else if (serial_reader.read_buf[1] == 0x91)
                {
                    steer = 0;
                    temp = serial_reader.read_buf[2];
                    temp <<= 24;
                    steer |= temp;
                    temp = serial_reader.read_buf[3];
                    temp <<= 16;
                    steer |= temp;
                    temp = serial_reader.read_buf[4];
                    temp <<= 8;
                    steer |= temp;
                    temp = serial_reader.read_buf[5];
                    steer |= temp;
                    std::cout << "steer " << steer << std::endl;
                }
            }
            else
            {
                std::cout << "Decrypted: ";
                for (unsigned i = 0; i < 6; ++i)
                {
                    std::cout << std::hex << (0xFF & (int)serial_reader.read_buf[i]);
                }
                std::cout << std::endl;
            }
        }
        else
        {
            if (arg2 == "3") // Only print ranging if mode 3 is selected
            {
                printf("%s", serial_reader.read_buf);
            }
        }

        serial_reader.clear_buf();
    }
}