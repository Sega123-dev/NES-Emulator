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

    uint8_t ppuReadRaw(uint16_t addr);
    void ppuWriteRaw(uint16_t addr, uint8_t data);

    Mapper *mapper;
    bool horizontalMirroring;
};