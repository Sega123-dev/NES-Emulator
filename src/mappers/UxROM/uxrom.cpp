#include <iostream>
#include <cstdint>
#include <vector>
#include "uxrom.hpp"

void UXROM::reset()
{
    currentPRGbank = 0;
}
UXROM::UXROM(std::vector<uint8_t> &prgData, std::vector<uint8_t> &chrData)
{
    chr = chrData;
    prg = prgData;

    currentPRGbank = 0;
}
uint8_t UXROM::ppuRead(uint16_t addr)
{
    if (addr < 0x2000)
    {
        return chr[addr];
    }
    return 0;
}
void UXROM::cpuWrite(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000)
    {
        currentPRGbank = data % (prg.size() / 0x4000 - 1);
    }
}
uint8_t UXROM::cpuRead(uint16_t addr)
{
    if (addr >= 0x8000 && addr <= 0xBFFF)
    {
        return prg[addr - 0x8000 + currentPRGbank * 0x4000];
    }
    else if (addr >= 0xC000 && addr <= 0xFFFF)
    {
        return prg[addr - 0xC000 + (prg.size() / 0x4000 - 1) * 0x4000];
    }
    return 0;
}
void UXROM::ppuWrite(uint16_t addr, uint8_t data)
{
    // No catching right now
}
