#include <iostream>
#include <cstdint>
#include <vector>
#include "cnrom.hpp"

void CNROM::reset()
{
    chrBankCount = 1;
    currentChrBank = 0;
}
CNROM::CNROM(std::vector<uint8_t> &prgData, std::vector<uint8_t> &chrData)
{
    prg = prgData;
    chr = chrData;

    chrBankCount = chr.size() / 0x2000;
    currentChrBank = 0;
}
uint8_t CNROM::cpuRead(uint16_t addr)
{
    if (prg.size() == 0x4000)
    {
        return prg[addr & 0x3FFF];
    }
    else
    {
        return prg[addr & 0x7FFF];
    }
}
void CNROM::cpuWrite(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000)
        currentChrBank = data % chrBankCount;
}
uint8_t CNROM::ppuRead(uint16_t addr)
{
    if (addr < 0x2000)
    {
        uint16_t newAddr = addr + (currentChrBank * 0x2000);
        return chr[newAddr];
    }
    return 0;
}
void CNROM::ppuWrite(uint16_t addr, uint8_t data)
{
    // No RAM
}
