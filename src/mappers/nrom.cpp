#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include "nrom.hpp"

enum Mirroring
{
    HORIZONTAL,
    VERTICAL
};
Mirroring mirroring;
NROM::NROM(const std::vector<uint8_t> &prgData, const std::vector<uint8_t> &chrData)
{
    prg = prgData;
    chr = chrData;

    if (prg.size() == 0x4000)
        prg16kb = true;
    else
        prg16kb = false;

    if (chr.size() == 0)
        chr8kb = false;
    else
        chr8kb = true;

    if (!chr8kb)
        chr.resize(0x2000);

    prgRam.resize(0x2000);
}

uint8_t NROM::cpuRead(uint16_t addr)
{
    if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        return prgRam[addr - 0x6000];
    }
    if (addr >= 0x8000)
    {
        uint32_t index = addr - 0x8000;
        if (prg16kb)
            index %= 0x4000;
        return prg[index];
    }
    return 0;
}
uint8_t NROM::ppuRead(uint16_t addr)
{
    if (addr < 0x2000)
    {
        return chr[addr];
    }
    return 0;
}
void NROM::ppuWrite(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000 && !chr8kb)
    {
        chr[addr] = data;
    }
}
void NROM::cpuWrite(uint16_t addr, uint8_t data)
{
    if (addr >= 0x6000 && addr <= 0x7FFF)
        prgRam[addr - 0x6000] = data;
}
uint16_t mirroringInfo(uint16_t addr)
{
    addr = addr & 0x0FFF;
    if (mirroring == VERTICAL)
    {
        return addr & 0x800;
    }
    else if (mirroring == HORIZONTAL)
    {
        return (addr / 2) & 0x400;
    }
}