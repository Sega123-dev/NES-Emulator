#pragma once
#include "../cpu/6502ricoh.hpp"
#include <cstdint>
class MMC3;
class Bus
{
public:
    Bus() = default;
    void write(uint16_t addr, uint8_t data);
    uint8_t read(uint16_t addr);

    CPU cpu;
    MMC3 *mmc3 = nullptr;
    uint8_t ram[2048];
};