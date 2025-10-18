#include "cpu/6502ricoh.hpp"
#include "bus/bus.hpp"
#include <iostream>

int main()
{
    Bus nes;
    CPU cpu;

    cpu.connectBus(&nes);
    cpu.reset();
}
