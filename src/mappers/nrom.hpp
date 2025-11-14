#pragma once
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <vector>

class Mapper
{
public:
    virtual uint8_t cpuRead(uint16_t addr) = 0;
    virtual void cpuWrite(uint16_t addr, uint8_t data) = 0;

    virtual uint8_t ppuRead(uint16_t addr) = 0;
    virtual void ppuWrite(uint16_t addr, uint8_t data) = 0;

    virtual ~Mapper() = default;
};

class NROM : public Mapper
{
public:
    std::vector<uint8_t> prg;
    std::vector<uint8_t> chr;
    std::vector<uint8_t> prgRam;
    bool prg16kb;
    bool chr8kb; // Checks if it's either CHR-RAM(0kb) or CHR-ROM(8kb)

    NROM(const std::vector<uint8_t> &prgData, const std::vector<uint8_t> &chrData);
    uint8_t cpuRead(uint16_t addr) override;
    uint8_t ppuRead(uint16_t addr) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;
    void NROM::cpuWrite(uint16_t addr, uint8_t data) override;
};