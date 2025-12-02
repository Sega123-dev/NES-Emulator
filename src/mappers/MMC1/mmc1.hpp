#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include "../mapper.hpp"
enum Mirroring
{
    VERTICAL,
    HORIZONTAL
};
class MMC1 : public Mapper
{
public:
    MMC1(const std::vector<uint8_t> &prgData, const std::vector<uint8_t> &chrData);
    uint8_t cpuRead(uint16_t addr) override;
    uint8_t ppuRead(uint16_t addr) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;
    void reset();

private:
    std::vector<uint8_t> prg;
    std::vector<uint8_t> chr;

    uint8_t shiftReg = 0;
    uint8_t shiftCount = 0;

    uint8_t controlReg = 0x0C;
    uint8_t chrBank0 = 0, chrBank1 = 0;
    uint8_t prgBank = 0;

    Mirroring mirroring = Mirroring::HORIZONTAL;
};
