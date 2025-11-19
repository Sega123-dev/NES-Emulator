#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <memory>
enum Mirroring
{
    VERTICAL,
    HORIZONTAL
};
class Mapper
{
public:
    virtual uint8_t cpuRead(uint16_t addr);
    virtual uint8_t ppuRead(uint16_t addr);
    virtual void cpuWrite(uint16_t addr, uint8_t data);
    virtual void ppuWrite(uint16_t addr, uint8_t data);

    ~Mapper() = default;
};
class CNROM : public Mapper
{
public:
    CNROM(std::vector<uint8_t> &prgData, std::vector<uint8_t> &chrData);
    uint8_t cpuRead(uint16_t addr) override;
    uint8_t ppuRead(uint16_t addr) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;
    void reset();

private:
    std::vector<uint8_t> prg;
    std::vector<uint8_t> chr;

    uint8_t currentChrBank = 0;
    uint8_t chrBankCount = 1;

    Mirroring mirroring = Mirroring::HORIZONTAL;
};