#pragma once
#include <cstdint>
#include <iostream>
#include <vector>
#include "../mappers/mapper.hpp"

class PPU
{
public:
    PPU(std::vector<uint8_t> &chrROMBuffer);
    uint8_t vram[2048];
    uint8_t paletteRAM[32];
    uint8_t oam[256];

    uint8_t secondaryOAM[8 * 4];

    static constexpr const uint32_t PPU_CLOCK_SPEED = 5369317;

    uint8_t PPUCTRL;
    uint8_t PPUMASK;
    uint8_t PPUSTATUS;
    uint8_t OAMADDR;

    std::vector<uint8_t> chrROM;

    uint16_t v; // Current VRAM addr
    uint16_t t; // Temp VRAM addr
    uint8_t x;  // Fine X scroll
    bool w;     // Write toggle

    void clock();

    void write2000(uint8_t data);
    void write2001(uint8_t data);
    uint8_t read2002();
    void write2005(uint8_t data);
    void write2006(uint8_t data);
    uint8_t read2007();
    void write2007(uint8_t data);

    int scanline;
    int cycle;

    bool vblankFlag;
    bool sprite0Hit;
    bool spriteOverflow;
    uint8_t ppuOpenBus;

    uint8_t spriteCount;
    uint8_t spritePatternLow[8];
    uint8_t spritePatternHigh[8];
    uint8_t spriteAttributes[8];

    uint8_t ppuReadRaw(uint16_t addr);
    void ppuWriteRaw(uint16_t addr, uint8_t data);

    Mapper *mapper;
    bool horizontalMirroring;

    // Write $2000

    int vramIncrement;
    uint16_t bgPatternTable;
    uint16_t spritePatternTable;
    int spriteHeight;

    bool nmiOutput;

    // Write $2001

    bool showBackground;
    bool showSprites;
    bool showLeftBackground;
    bool showLeftSprites;
    bool grayscaleMode;

    bool emphasizeRed;
    bool emphasizeGreen;
    bool emphasizeBlue;

    uint8_t framebuffer[240][256]; // Screen

    uint16_t bgShiftLow = 0;
    uint16_t bgShiftHigh = 0;
    uint16_t attrShiftLow = 0;
    uint16_t attrShiftHigh = 0;

    uint8_t nextTileID = 0;
    uint8_t nextTileAttr = 0;
    uint8_t nextTileLow = 0;
    uint8_t nextTileHigh = 0;

    // Background shift register helpers
    void loadBackgroundShifters();

    // Scrolling helpers
    void incrementX();
    void incrementY();
    void copyHorizontal();
    void copyVertical();
};