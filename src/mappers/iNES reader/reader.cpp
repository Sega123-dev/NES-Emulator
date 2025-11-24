#include "reader.hpp"
#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>

void dumpROM(const char *filename)
{
    std::vector<char> headerBuffer;
    std::vector<char> prgBuffer;
    std::vector<char> chrBuffer;
    std::ifstream rom(filename, std::ios::binary);
    headerBuffer.resize(16);
    if (rom.fail())
    {
        std::cerr << "Failed to run ROM(Error:NotFound).";
        return;
    }
    if (rom.read(headerBuffer.data(), 16))
    {
        for (int i = 0; i < 16; i++)
        {
            header[i] = static_cast<uint8_t>(headerBuffer[i]);
        }
    }
    if (header[0] == 0x4E && header[1] == 0x45 && header[2] == 0x53 && header[3] == 0x1A)
    {
        flag6 = header[6];
        flag7 = header[7];
        prgRomSize = header[4] * 0x4000;
        chrRomSize = header[5] * 0x2000;

        prgBuffer.resize(prgRomSize);
        chrBuffer.resize(chrRomSize);

        verticalMirroring = (flag6 & 1);
        SRAM = (flag6 & 2);
        fourScreenMirroring = (flag6 & 8);

        mapperID = ((flag7 & 0xF0) | (flag6 >> 4));

        bool trainer = (flag6 & 4) != 0;
        if (trainer)
            rom.seekg(512, std::ios::cur);

        if (rom.read(prgBuffer.data(), prgRomSize))
        {
            for (int i = 0; i < prgRomSize; i++)
            {
                PRG_ROM[i] = static_cast<uint8_t>(prgBuffer[i]);
            }
        }
        if (rom.read(chrBuffer.data(), chrRomSize))
        {
            for (int i = 0; i < chrRomSize; i++)
            {
                CHR_ROM[i] = static_cast<uint8_t>(chrBuffer[i]);
            }
        }
    }
    else
    {
        std::cerr << "This file is not NES ROM file.Please provide a file that is using iNES format.";
    }
}
