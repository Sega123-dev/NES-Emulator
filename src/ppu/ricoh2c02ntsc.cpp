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
void PPU::clock()
{
    cycle++;
    if (cycle > 340)
    {
        cycle = 0;
        scanline++;
        if (scanline > 261)
            scanline = 0;
    }
    if (scanline == 241 && cycle == 1)
        vblankFlag = true;
    if (scanline == 261 && cycle == 1)
    {
        vblankFlag = false;
        sprite0Hit = false;
        spriteOverflow = false;
    }
    if (scanline >= 0 && scanline <= 239)
    {
        uint16_t tileAddress = 0x2000 | (v & 0x0FFF);
        uint8_t tileID = ppuReadRaw(tileAddress);
        uint8_t fineY = (v & 0x7000) >> 12;

        uint8_t attrTableAddr = 0x23C0 + (v & 0x0C00) + ((v >> 4) & 0x38) + ((v >> 2) & 0x07);
        uint8_t lowPtrTableByteAddr = bgPatternTable + (tileID * 16) + fineY;
        uint8_t highPtrTableByteAddr = bgPatternTable + (tileID * 16) + fineY + 8;

        uint8_t attrByte = ppuReadRaw(attrTableAddr);

        uint8_t lowByte = ppuReadRaw(lowPtrTableByteAddr);
        uint8_t highByte = ppuReadRaw(highPtrTableByteAddr);

        uint8_t coarseX = (v & 0x1F);
        uint8_t coarseY = (v >> 5) & 0x1F;

        uint8_t quadX = (coarseX % 4) / 2;
        uint8_t quadY = (coarseY % 4) / 2;

        uint8_t paletteBits = (attrByte >> ((quadY * 2 + quadX) * 2)) & 0x03;
        uint8_t fineX = x;
        uint8_t pixel = fineX;

        uint8_t colorIndex = ((highByte >> (7 - pixel)) & 1) << 1 |
                             ((lowByte >> (7 - pixel)) & 1);

        colorIndex |= (paletteBits << 2);

        framebuffer[scanline][cycle - 1] = colorIndex;

        if ((v & 0x001F) == 31)
        {
            v &= ~0x001F;
            v ^= 0x0400;
        }
        else
        {
            v += 1;
        }
        if ((v & 0x7000) != 0x7000)
        {
            v += 0x1000;
        }
        else
        {
            v &= ~0x7000;
            uint16_t coarseY = (v & 0x03E0) >> 5;
            if (coarseY == 29)
            {
                coarseY = 0;
                v ^= 0x0800;
            }
            else if (coarseY == 31)
            {
                coarseY = 0;
            }
            else
            {
                coarseY += 1;
            }
            v = (v & ~0x03E0) | (coarseY << 5);
        }
        spriteCount = 0;
        for (int i = 0; i < 64; i++)
        {
            int y = oam[i * 4];

            if (scanline >= y && scanline < y + spriteHeight)
            {
                if (spriteCount < 8)
                {
                    for (int j = 0; j < 4; j++)
                        secondaryOAM[spriteCount * 4 + j] = oam[i * 4 + j];
                    spriteCount++;
                }
                else
                {
                    spriteOverflow = true;
                    break;
                }
            }
        }
        for (uint8_t i = 0; i < spriteCount; i++)
        {
            uint8_t tileID = secondaryOAM[i * 4 + 1];
            uint8_t attributes = secondaryOAM[i * 4 + 2];
            uint8_t spriteX = secondaryOAM[i * 4 + 3];
            uint8_t spriteY = secondaryOAM[i * 4 + 0];

            uint8_t fineY = scanline - spriteY;
            uint16_t addr = spritePatternTable + tileID * 16 + fineY;

            spritePatternLow[i] = ppuReadRaw(addr);
            spritePatternHigh[i] = ppuReadRaw(addr + 8);
            spriteAttributes[i] = attributes;

            int xPos = spriteX;
            if (cycle - 1 >= xPos && cycle - 1 < xPos + 8)
            {
                uint8_t fineX = cycle - 1 - xPos;
                if (attributes & 0x40)
                    fineX = 7 - fineX;

                uint8_t spritePixel = ((spritePatternHigh[i] >> (7 - fineX)) & 1) << 1 |
                                      ((spritePatternLow[i] >> (7 - fineX)) & 1);
                if (spritePixel != 0)
                {
                    uint8_t palette = (attributes & 0x03) << 2;
                    bool behindBackground = attributes & 0x20;
                    if (!behindBackground || framebuffer[scanline][cycle - 1] == 0)
                    {
                        framebuffer[scanline][cycle - 1] = palette | spritePixel;
                    }

                    if (i == 0 && framebuffer[scanline][cycle - 1] != 0)
                        sprite0Hit = true;
                }
            }
        }
    }
}
