#include "6502ricoh.hpp"
#include "../bus/bus.hpp"

void CPU::reset()
{
    bus = nullptr;
    dataBus = 0;
    A = 0;
    X = 0;
    Y = 0;
    P = 0x24;
    pc = 0;
    sp = 0xFD;
    cycles = 0;
    opState = 0;
    NMI = IRQ = RESET = false;
}

void CPU::clock()
{
    if (cycles == 0)
    {
        uint8_t opcode = bus->read(pc);
        Instruction &instr = table[opcode];

        (this->*instr.execute)();
        cycles = instr.cycles;
    }
    cycles--;
}
void CPU::connectBus(Bus *b)
{
    bus = b;
}