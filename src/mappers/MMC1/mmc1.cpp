#include "mmc1.hpp"
#include <iostream>
#include <cstdint>
#include <vector>
void MMC1::reset()
{
    shiftReg = 0;
    shiftCount = 0;

    controlReg = 0x0C;
    chrBank0 = 0, chrBank1 = 0;
    prgBank = 0;

    mirroring = Mirroring::HORIZONTAL;
}
MMC1::MMC1(const std::vector<uint8_t> &prgData, const std::vector<uint8_t> &chrData)
{
    prg = prgData;
    chr = chrData;

    reset();
};
void MMC1::cpuWrite(uint16_t addr, uint8_t value)
{
    if (value & 0x80)
    {
        controlReg |= 0x0C;
        shiftReg = 0;
        shiftCount = 0;
        return;
    }

    shiftReg >>= 1;
    shiftReg |= (value & 1) << 4;
    shiftCount++;

    if (shiftCount == 5)
    {
        if (addr >= 0x8000 && addr <= 0x9FFF)
            controlReg = shiftReg;
        else if (addr >= 0xA000 && addr <= 0xBFFF)
            chrBank0 = shiftReg;
        else if (addr >= 0xC000 && addr <= 0xDFFF)
            chrBank1 = shiftReg;
        else if (addr >= 0xE000 && addr <= 0xFFFF)
            prgBank = shiftReg;

        shiftReg = 0;
        shiftCount = 0;

        switch (controlReg & 0x03)
        {
        case 0:
            mirroring = VERTICAL;
            break;
        case 1:
            mirroring = VERTICAL;
            break;
        case 2:
            mirroring = VERTICAL;
            break;
        case 3:
            mirroring = HORIZONTAL;
            break;
        }
    };
}
uint8_t MMC1::cpuRead(uint16_t addr)
{
    if (addr < 0x8000)
        return 0;
    size_t index = 0;
    uint8_t prgMode = (controlReg >> 2) & 0x03;

    switch (prgMode)
    {
    case 0:
    case 1:
        index = (prgBank & 0xFE) * 0x4000 + (addr - 0x8000);
        break;
    case 2:
        if (addr < 0xC000)
            index = addr - 0x8000;
        else
            index = prgBank * 0x4000 + (addr - 0xC000);
        break;
    case 3:
        if (addr < 0xC000)
            index = prgBank * 0x4000 + (addr - 0x8000);
        else
            index = prg.size() - 0x4000 + (addr - 0xC000);
        break;
    }
    return prg[index % prg.size()];
}
uint8_t MMC1::ppuRead(uint16_t addr)
{
    if (addr < 0x1000)
        return chr[(chrBank0 * 0x1000 + addr) % chr.size()];
    else if (addr < 0x2000)
        return chr[(chrBank1 * 0x1000 + (addr - 0x1000)) % chr.size()];
    return 0;
}
void MMC1::ppuWrite(uint16_t addr, uint8_t data)
{
    if (addr < 0x1000)
        chr[(chrBank0 * 0x1000 + addr) % chr.size()] = data;
    else if (addr < 0x2000)
        chr[(chrBank1 * 0x1000 + (addr - 0x1000)) % chr.size()] = data;
}

// MMC1 simplified,may not enough features