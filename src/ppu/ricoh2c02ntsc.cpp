#include <cstdint>
#include <iostream>
#include <vector>
#include "ricoh2c02ntsc.hpp"
#include "../mappers/iNES_reader/reader.hpp"
#include "../mappers/mapper.hpp"

PPU::PPU(std::vector<uint8_t> &chrROMBuffer)
{
    chrROM = chrROMBuffer;

    for (int i = 0; i < 2048; i++)
        vram[i] = 0;
    for (int i = 0; i < 32; i++)
        paletteRAM[i] = 0;
    for (int i = 0; i < 256; i++)
        oam[i] = 0;

    PPUCTRL = 0;
    PPUMASK = 0;
    PPUSTATUS = 0xA0;
    OAMADDR = 0;

    v = 0;
    t = 0;
    x = 0;
    w = false;

    scanline = -1;
    cycle = 0;

    horizontalMirroring = false; // Pull exact mirroring behavior from a mapper,this variable is a placeholder
}

uint8_t PPU ::ppuReadRaw(uint16_t addr)
{
    addr = addr & 0x3FFF;

    if (addr >= 0x3000 && addr <= 0x3EFF)
        addr -= 0x1000;

    unsigned int VRAMindex;

    if (addr >= 0x0000 && addr <= 0x1FFF)
        return mapper->ppuRead(addr);

    if (addr >= 0x2000 && addr <= 0x2FFF)
    {
        uint8_t NTindex = (addr - 0x2000) / 0x400;
        unsigned int offset = (addr - 0x2000) % 0x400;

        if (horizontalMirroring)
        {
            VRAMindex = (NTindex / 2) * 0x400 + offset;
        }
        else
        {
            VRAMindex = (NTindex % 2) * 0x400 + offset;
        }

        return vram[VRAMindex];
    }

    if (addr >= 0x3F00 && addr <= 0x3FFF)
    {
        addr = 0x3F00 + (addr % 0x20);

        if (addr == 0x3F10)
            addr = 0x3F00;
        if (addr == 0x3F14)
            addr = 0x3F04;
        if (addr == 0x3F18)
            addr = 0x3F08;
        if (addr == 0x3F1C)
            addr = 0x3F0C;

        return paletteRAM[addr - 0x3F00];
    }
    return 0;
}

void PPU::ppuWriteRaw(uint16_t addr, uint8_t data)
{
    addr = addr & 0x3FFF;

    if (addr >= 0x3000 && addr <= 0x3EFF)
        addr -= 0x1000;

    unsigned int VRAMindex;

    if (addr >= 0x0000 && addr <= 0x1FFF)
        mapper->ppuWrite(addr, data);

    if (addr >= 0x2000 && addr <= 0x2FFF)
    {
        uint8_t NTindex = (addr - 0x2000) / 0x400;
        unsigned int offset = (addr - 0x2000) % 0x400;

        if (horizontalMirroring)
        {
            VRAMindex = (NTindex / 2) * 0x400 + offset;
        }
        else
        {
            VRAMindex = (NTindex % 2) * 0x400 + offset;
        }

        vram[VRAMindex] = data;
    }

    if (addr >= 0x3F00 && addr <= 0x3FFF)
    {
        addr = 0x3F00 + (addr % 0x20);

        if (addr == 0x3F10)
            addr = 0x3F00;
        if (addr == 0x3F14)
            addr = 0x3F04;
        if (addr == 0x3F18)
            addr = 0x3F08;
        if (addr == 0x3F1C)
            addr = 0x3F0C;

        paletteRAM[addr - 0x3F00] = data;
    }
    ppuOpenBus = data;
}

void PPU::write2000(uint8_t data)
{
    PPUCTRL = data;

    t = (t & 0x73FF) | ((data & 0x03) << 10);

    vramIncrement = (data & 0x04) ? 32 : 1;

    bgPatternTable = (data & 0x10) ? 0x1000 : 0x0000;

    spritePatternTable = (data & 0x08) ? 0x1000 : 0x0000;

    spriteHeight = (data & 0x20) ? 16 : 8;

    nmiOutput = (data & 0x80) != 0; // Update the NMI in the CPU when VBLANK is set
}
void PPU::write2001(uint8_t data)
{
    PPUMASK = data;

    grayscaleMode = data & 0x01; // Set it during the read/write

    showLeftBackground = data & 0x02;
    showLeftSprites = data & 0x04;
    showBackground = data & 0x08;
    showSprites = data & 0x10;

    emphasizeRed = data & 0x20;
    emphasizeGreen = data & 0x40;
    emphasizeBlue = data & 0x80;
}
uint8_t PPU::read2002()
{
    PPUSTATUS = (vblankFlag << 7) | (sprite0Hit << 6) | (spriteOverflow << 5) | (ppuOpenBus & 0x1F);
    vblankFlag = false;
    w = false;
    return PPUSTATUS;
}