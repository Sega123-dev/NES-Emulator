#include "reader.hpp"
#include "NROM/nrom.hpp"
#include "CNROM/cnrom.hpp"
#include "UxROM/uxrom.hpp"
#include "MMC1/mmc1.hpp"
#include "MMC3/mmc3.hpp"

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
        std::cerr << "Failed to run ROM(Error:NotFound)." << '\n';
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
        rom.close();
        Mapper *mapper = chooseMapper();
        if (!mapper)
            return;
    }
    else
    {
        std::cerr << "This file is not NES ROM file.Please provide a file that is using iNES format." << '\n';
    }
}

Mapper *chooseMapper()
{
    switch (mapperID)
    {
    case 0:
        return new NROM(PRG_ROM, CHR_ROM);
    case 1:
        return new MMC1(PRG_ROM, CHR_ROM);
    case 2:
        return new UXROM(PRG_ROM, CHR_ROM);
    case 3:
        return new CNROM(PRG_ROM, CHR_ROM);
    case 4:
        return new MMC3(PRG_ROM, CHR_ROM);
    default:
        std::cerr << "NEScape does not support mapper " << mapperID << '\n';
        return nullptr;
    }
}