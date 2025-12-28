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

// HELPERS

void PPU::loadBackgroundShifters()
{
    bgShiftLow = (bgShiftLow & 0xFF00) | nextTileLow;
    bgShiftHigh = (bgShiftHigh & 0xFF00) | nextTileHigh;

    uint8_t palette =
        (nextTileAttr >>
         (((v >> 4) & 4) | (v & 2))) &
        0x03;

    attrShiftLow = (attrShiftLow & 0xFF00) | ((palette & 1) ? 0xFF : 0x00);
    attrShiftHigh = (attrShiftHigh & 0xFF00) | ((palette & 2) ? 0xFF : 0x00);
}

void PPU::incrementX()
{
    if ((v & 0x001F) == 31)
    {
        v &= ~0x001F;
        v ^= 0x0400;
    }
    else
        v++;
}

void PPU::incrementY()
{
    if ((v & 0x7000) != 0x7000)
        v += 0x1000;
    else
    {
        v &= ~0x7000;
        uint16_t y = (v & 0x03E0) >> 5;
        if (y == 29)
        {
            y = 0;
            v ^= 0x0800;
        }
        else if (y == 31)
            y = 0;
        else
            y++;
        v = (v & ~0x03E0) | (y << 5);
    }
}

void PPU::copyHorizontal()
{
    v = (v & ~0x041F) | (t & 0x041F);
}

void PPU::copyVertical()
{
    v = (v & ~0x7BE0) | (t & 0x7BE0);
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
void PPU::write2006(uint8_t data)
{
    if (w == 0)
    {
        t = (t & 0x00FF) | ((data & 0x3F) << 8);
        w = 1;
    }
    else
    {
        t = (t & 0xFF00) | data;
        v = t;
        w = 0;
    }
}
void PPU::clock()
{
    spriteHeight = (PPUCTRL & 0x20) ? 16 : 8; // Needs to be set before any first visible scanlines

    bool renderingEnabled = showBackground || showSprites;

    if (scanline >= -1 && scanline < 240)
    {
        if (scanline == -1 && cycle == 1)
        {
            vblankFlag = false;
            sprite0Hit = false;
            spriteOverflow = false;
        }
        if (renderingEnabled &&
            cycle >= 1 && cycle <= 256)
        {
            bgShiftLow <<= 1;
            bgShiftHigh <<= 1;
            attrShiftLow <<= 1;
            attrShiftHigh <<= 1;
        }
        if (renderingEnabled &&
            ((cycle >= 1 && cycle <= 256) || (cycle >= 321 && cycle <= 336)))
        {
            switch (cycle % 8)
            {
            case 1:
                nextTileID = ppuReadRaw(0x2000 | (v & 0x0FFF));
                break;

            case 3:
                nextTileAttr = ppuReadRaw(
                    0x23C0 |
                    (v & 0x0C00) |
                    ((v >> 4) & 0x38) |
                    ((v >> 2) & 0x07));
                break;

            case 5:
                nextTileLow = ppuReadRaw(
                    bgPatternTable +
                    nextTileID * 16 +
                    ((v >> 12) & 7));
                break;

            case 7:
                nextTileHigh = ppuReadRaw(
                    bgPatternTable +
                    nextTileID * 16 +
                    ((v >> 12) & 7) + 8);
                break;

            case 0:
                loadBackgroundShifters();
                incrementX();
                break;
            }
        }
        if (renderingEnabled)
        {
            if (cycle == 256)
                incrementY();

            if (cycle == 257)
                copyHorizontal();

            if (scanline == -1 && cycle >= 280 && cycle <= 304)
                copyVertical();
        }
    }
    if (scanline >= 0 && scanline < 240 &&
        cycle >= 1 && cycle <= 256)
    {
        uint8_t bit = 15 - x;

        uint8_t bgPixel =
            ((bgShiftHigh >> bit) & 1) << 1 |
            ((bgShiftLow >> bit) & 1);

        uint8_t bgPalette =
            ((attrShiftHigh >> bit) & 1) << 1 |
            ((attrShiftLow >> bit) & 1);

        uint8_t colorIndex =
            bgPixel ? ((bgPalette << 2) | bgPixel) : 0;

        framebuffer[scanline][cycle - 1] = colorIndex;
    }
    if (scanline == 241 && cycle == 1)
        vblankFlag = true;
    cycle++;
    if (cycle > 340)
    {
        cycle = 0;
        scanline++;
        if (scanline > 261)
            scanline = -1;
    }
    if (scanline >= 257 && scanline <= 320)
    {

        for (int i = 0; i < 32; i++)
        {
            secondaryOAM[i] = 0;
        }
        spriteCount = 0;
        for (int i = 0; i < 64; i++)
        {
            uint8_t Ypos = oam[i * 4 + 0];

            if (scanline >= Ypos && scanline < Ypos + spriteHeight)
            {
                spriteAttributes[i] = secondaryOAM[i * 4 + 2];
                spriteX[i] = secondaryOAM[i * 4 + 3];
                if (spriteCount >= 8)
                {
                    spriteOverflow = true;
                    break;
                }
                for (int j = 0; j < 4; j++)
                {
                    secondaryOAM[spriteCount * 4 + j] = oam[i * 4 + j];
                }
                spriteCount++;
            }
        }
        for (int i = 0; i < spriteCount; i++)
        {
            uint8_t spriteY = secondaryOAM[i * 4 + 0];
            uint8_t Xsprite = secondaryOAM[i * 4 + 3];
            uint8_t spriteRow = scanline - spriteY;
            uint16_t patternTableBase;
            uint8_t tileIndex = secondaryOAM[i * 4 + 1];
            uint16_t patternTableAddr;

            if (secondaryOAM[i * 4 + 2] & 0x80)
                spriteRow = (spriteHeight - 1) - spriteRow;

            spriteRowBuffer[i] = spriteRow;

            if (spriteHeight == 8)
            {
                patternTableBase = (PPUCTRL & 0x08) ? 0x1000 : 0x0000;
                patternTableAddr = patternTableBase + tileIndex * 16 + spriteRow;
            }
            else
            {
                patternTableBase = (tileIndex & 1) ? 0x1000 : 0x0000;

                if (spriteRow < 8)
                    patternTableAddr = patternTableBase + (tileIndex & 0xFE) * 16 + spriteRow;
                else
                    patternTableAddr = patternTableBase + (tileIndex | 0x01) * 16 + (spriteRow - 8);
            }

            spritePatternLow[i] = ppuReadRaw(patternTableAddr);
            spritePatternHigh[i] = ppuReadRaw(patternTableAddr + 8);

            int fineX = cycle - Xsprite;
            if (fineX < 0 || fineX > 7)
                continue;

            if (secondaryOAM[i * 4 + 2] & 0x40)
                fineX = 7 - fineX;

            uint8_t spritePixel = ((spritePatternHigh[i] >> (7 - fineX)) & 1) << 1 |
                                  ((spritePatternLow[i] >> (7 - fineX)) & 1);
        }
    }
}
