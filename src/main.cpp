#include "cpu/6502ricoh.hpp"
#include "bus/bus.hpp"
#include "mappers/MMC3/mmc3.hpp"
#include <iostream>

int main()
{
    Bus nes;
    CPU cpu;
    MMC3 mmc3;
    nes.cpu.connectBus(&nes);
    nes.mmc3.connectBus(&nes);
    cpu.reset();
}
