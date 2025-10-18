#include "bus.hpp"

void Bus::write(uint16_t addr, uint8_t data)
{
    if (addr >= 0x0000 && addr <= 0x1FFF)
        ram[addr & 0x07FF] = data;
}
uint8_t Bus::read(uint16_t addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF)
        return ram[addr & 0x07FF];
    return 0x00;
}