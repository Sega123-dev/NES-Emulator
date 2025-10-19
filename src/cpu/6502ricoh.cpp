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

void CPU::ldaImmediate()
{
    uint8_t value = bus->read(pc++);
    A = value;

    if ((A & 0x80) != 0)
        P |= 0x80;
    else
        P &= ~0x80;
    if (A == 0)
        P |= 0x02;
    else
        P &= ~0x02;
}

void CPU::ldaZeroPage()
{
    uint8_t address = bus->read(pc++);
    uint8_t value = bus->read(address);

    A = value;

    if ((A & 0x80) != 0)
        P |= 0x80;
    else
        P &= ~0x80;
    if (A == 0)
        P |= 0x02;
    else
        P &= ~0x02;
}

void CPU::ldaZeroPageX()
{
    uint8_t address = bus->read(pc++);
    uint8_t zeroPageAddress = (address + X) & 0xFF;
    uint8_t value = bus->read(zeroPageAddress);

    A = value;

    if ((A & 0x80) != 0)
        P |= 0x80;
    else
        P &= ~0x80;
    if (A == 0)
        P |= 0x02;
    else
        P &= ~0x02;
}

void CPU::ldaAbsolute()
{
    uint8_t lowByte = bus->read(pc++);
    uint8_t highByte = bus->read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint8_t value = bus->read(finalAddress);

    A = value;
    if ((A & 0x80) != 0)
        P |= 0x80;
    else
        P &= ~0x80;
    if (A == 0)
        P |= 0x02;
    else
        P &= ~0x02;
}

void CPU::ldaAbsoluteX()
{
    uint8_t lowByte = bus->read(pc++);
    uint8_t highByte = bus->read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint16_t finalAddressX = finalAddress + X;
    uint8_t value = bus->read(finalAddressX);

    A = value;

    if ((finalAddress & 0xFF00) != (finalAddressX & 0xFF00))
        cycles++;

    A = value;
    if ((A & 0x80) != 0)
        P |= 0x80;
    else
        P &= ~0x80;
    if (A == 0)
        P |= 0x02;
    else
        P &= ~0x02;
}

void CPU::ldaAbsoluteY()
{
    uint8_t lowByte = bus->read(pc++);
    uint8_t highByte = bus->read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint16_t finalAddressY = finalAddress + Y;
    uint8_t value = bus->read(finalAddressY);

    A = value;

    if ((finalAddress & 0xFF00) != (finalAddressY & 0xFF00))
        cycles++;

    if ((A & 0x80) != 0)
        P |= 0x80;
    else
        P &= ~0x80;
    if (A == 0)
        P |= 0x02;
    else
        P &= ~0x02;
}

void CPU::ldaIndexedIndirect()
{
    uint8_t address = bus->read(pc++);
    uint8_t zeroPageAddress = (address + X) & 0xFF;
    uint8_t lowByte = bus->read(zeroPageAddress);
    uint8_t highByte = bus->read((zeroPageAddress + 1) & 0xFF);
    uint16_t ptr = (highByte << 8) | lowByte;
    A = bus->read(ptr);

    if ((A & 0x80) != 0)
        P |= 0x80;
    else
        P &= ~0x80;
    if (A == 0)
        P |= 0x02;
    else
        P &= ~0x02;
}
void CPU::ldaIndirectIndexed()
{
    uint8_t address = bus->read(pc++);
    uint8_t lowByte = bus->read(address);
    uint8_t highByte = bus->read((address + 1) & 0xFF);
    uint16_t ptr = (highByte << 8) | lowByte;
    uint16_t finalAddress = ptr + Y;
    A = bus->read(finalAddress);

    if ((A & 0x80) != 0)
        P |= 0x80;
    else
        P &= ~0x80;
    if (A == 0)
        P |= 0x02;
    else
        P &= ~0x02;
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
