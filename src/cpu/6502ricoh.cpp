#include "6502ricoh.hpp"
#include "../bus/bus.hpp"
#include <cstdlib>

// HELPERS

void CPU::setNZ(uint8_t reg)
{
    if ((reg & 0x80) != 0)
        P |= 0x80;
    else
        P &= ~0x80;
    if (reg == 0)
        P |= 0x02;
    else
        P &= ~0x02;
}

void CPU::setVC(uint8_t V, uint8_t C)
{
    if (C)
        P |= 0x01;
    else
        P &= ~0x01;

    if (V)
        P |= 0x40;
    else
        P &= ~0x40;
}

// REST OF THE CPU

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
void CPU::brk()
{
    pc++;

    bus->write(0x0100 + sp, (pc >> 8) & 0xFF);
    sp--;

    bus->write(0x0100 + sp, pc & 0xFF);
    sp--;

    P |= 0x30;
    bus->write(0x0100 + sp, P);
    sp--;

    P |= 0x04;

    uint8_t lowByte = bus->read(0xFFFE);
    uint8_t highByte = bus->read(0xFFFF);
    pc = (highByte << 8) | lowByte;
}
void CPU::nop()
{
    pc++;
}
void CPU::rti()
{
    sp++;
    P = bus->read(0x0100 + sp);

    P &= ~0x10;
    P |= 0x20;

    sp++;
    uint8_t lowByte = bus->read(0x0100 + sp);
    sp++;
    uint8_t highByte = bus->read(0x0100 + sp);

    uint16_t subAddress = (highByte << 8) | lowByte;
    pc = subAddress;
}
void CPU::inx()
{
    X++;

    setNZ(X);
}
void CPU::iny()
{
    Y++;

    setNZ(Y);
}
void CPU::dex()
{
    X--;

    setNZ(X);
}
void CPU::dey()
{
    Y--;

    setNZ(Y);
}
void CPU::clc()
{
    P &= ~0x01;
}
void CPU::sec()
{
    P |= 0x01;
}
void CPU::cld()
{
    P &= ~0x08;
}
void CPU::cli()
{
    P &= ~0x04;
}
void CPU::sei()
{
    P |= 0x04;
}
void CPU::ldaImmediate()
{
    uint8_t value = bus->read(pc++);
    A = value;

    setNZ(A);
}
void CPU::ldaZeroPage()
{
    uint8_t address = bus->read(pc++);
    uint8_t value = bus->read(address);

    A = value;

    setNZ(A);
}
void CPU::ldaZeroPageX()
{
    uint8_t address = bus->read(pc++);
    uint8_t zeroPageAddress = (address + X) & 0xFF;
    uint8_t value = bus->read(zeroPageAddress);

    A = value;

    setNZ(A);
}
void CPU::ldaAbsolute()
{
    uint8_t lowByte = bus->read(pc++);
    uint8_t highByte = bus->read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint8_t value = bus->read(finalAddress);

    A = value;
    setNZ(A);
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
    setNZ(A);
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

    setNZ(A);
}
void CPU::ldaIndexedIndirect()
{
    uint8_t address = bus->read(pc++);
    uint8_t zeroPageAddress = (address + X) & 0xFF;
    uint8_t lowByte = bus->read(zeroPageAddress);
    uint8_t highByte = bus->read((zeroPageAddress + 1) & 0xFF);
    uint16_t ptr = (highByte << 8) | lowByte;
    A = bus->read(ptr);

    setNZ(A);
}
void CPU::ldaIndirectIndexed()
{
    uint8_t address = bus->read(pc++);
    uint8_t lowByte = bus->read(address);
    uint8_t highByte = bus->read((address + 1) & 0xFF);
    uint16_t ptr = (highByte << 8) | lowByte;
    uint16_t finalAddress = ptr + Y;
    A = bus->read(finalAddress);

    setNZ(A);
}
void CPU::ldxImmediate()
{
    uint8_t value = bus->read(pc++);
    X = value;

    setNZ(X);
}
void CPU::ldxZeroPage()
{
    uint8_t address = bus->read(pc++);
    uint8_t value = bus->read(address);

    X = value;

    setNZ(X);
}
void CPU::ldxZeroPageY()
{
    uint8_t address = bus->read(pc++);
    uint8_t zeroPageAddress = (address + Y) & 0xFF;
    uint8_t value = bus->read(zeroPageAddress);

    X = value;

    setNZ(X);
}
void CPU::ldxAbsolute()
{
    uint8_t lowByte = bus->read(pc++);
    uint8_t highByte = bus->read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint8_t value = bus->read(finalAddress);

    X = value;

    setNZ(X);
}
void CPU::ldxAbsoluteY()
{
    uint8_t lowByte = bus->read(pc++);
    uint8_t highByte = bus->read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint16_t finalAddressY = finalAddress + Y;
    uint8_t value = bus->read(finalAddressY);

    X = value;

    if ((finalAddress & 0xFF00) != (finalAddressY & 0xFF00))
        cycles++;

    setNZ(X);
}
void CPU::ldyImmediate()
{
    uint8_t value = bus->read(pc++);
    Y = value;

    setNZ(Y);
}
void CPU::ldyZeroPage()
{
    uint8_t address = bus->read(pc++);
    uint8_t value = bus->read(address);

    Y = value;

    setNZ(Y);
}
void CPU::ldyZeroPageX()
{
    uint8_t address = bus->read(pc++);
    uint8_t zeroPageAddress = (address + X) & 0xFF;
    uint8_t value = bus->read(zeroPageAddress);

    Y = value;

    setNZ(Y);
}
void CPU::ldyAbsolute()
{
    uint8_t lowByte = bus->read(pc++);
    uint8_t highByte = bus->read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint8_t value = bus->read(finalAddress);

    Y = value;

    setNZ(Y);
}
void CPU::ldyAbsoluteX()
{
    uint8_t lowByte = bus->read(pc++);
    uint8_t highByte = bus->read(pc++);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    uint16_t finalAddressX = finalAddress + X;
    uint8_t value = bus->read(finalAddressX);

    Y = value;

    if ((finalAddress & 0xFF00) != (finalAddressX & 0xFF00))
        cycles++;

    setNZ(Y);
}
void CPU::staAbsolute()
{
    uint8_t lowByte = bus->read(pc);
    uint8_t highByte = bus->read(pc + 1);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    pc += 2;
    bus->write(finalAddress, A);
}
void CPU::staAbsoluteX()
{
    uint8_t lowByte = bus->read(pc);
    uint8_t highByte = bus->read(pc + 1);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    pc += 2;
    bus->write(finalAddress + X, A);
}
void CPU::staAbsoluteY()
{
    uint8_t lowByte = bus->read(pc);
    uint8_t highByte = bus->read(pc + 1);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    pc += 2;
    bus->write(finalAddress + Y, A);
}
void CPU::staZeroPage()
{
    uint8_t address = bus->read(pc);
    pc += 1;
    bus->write(address, A);
}
void CPU::staZeroPageX()
{
    uint8_t address = bus->read(pc);
    pc += 1;
    bus->write((address + X) & 0xFF, A);
}
void CPU::staIndexedIndirect()
{
    uint8_t baseAddress = bus->read(pc);
    pc += 1;
    uint16_t ptr = (baseAddress + X) & 0xFF;
    uint8_t lowByte = bus->read(ptr);
    uint8_t highByte = bus->read((ptr + 1) & 0xFF);
    uint16_t finalAddress = (highByte << 8) | lowByte;

    bus->write(finalAddress, A);
}
void CPU::staIndirectIndexed()
{
    uint8_t baseAddress = bus->read(pc);
    pc += 1;
    uint8_t lowByte = bus->read(baseAddress);
    uint8_t highByte = bus->read((baseAddress + 1) & 0xFF);
    uint16_t finalAddress = (highByte << 8) | lowByte;
    finalAddress += Y;
    bus->write(finalAddress, A);
}
void CPU::tax()
{
    X = A;
    setNZ(X);
}
void CPU::tay()
{
    Y = A;
    setNZ(Y);
}
void CPU::txa()
{
    A = X;
    setNZ(A);
}
void CPU::tya()
{
    A = Y;
    setNZ(A);
}
void CPU::tsx()
{
    X = sp;
    setNZ(X);
}
void CPU::txs()
{
    sp = X;
}
void CPU::pha()
{
    bus->write(0x0100 + sp, A);
    sp--;
}
void CPU::pla()
{
    sp++;
    A = bus->read(0x0100 + sp);

    if (A == 0)
        P |= 0x02;
    else
        P &= ~0x02;

    if (A & 0x80)
        P |= 0x80;
    else
        P &= ~0x80;
}
void CPU::php()
{
    bus->write(0x0100 + sp, P | 0x30);
    sp--;
}
void CPU::plp()
{
    sp++;
    P = bus->read(0x0100 + sp);

    P &= 0xEF;
    P |= 0x20;
}
void CPU::jmpAbsolute()
{
    uint8_t lowByte = bus->read(pc);
    uint8_t highByte = bus->read(pc + 1);
    uint16_t finalAddress = (highByte << 8) | lowByte;

    pc = finalAddress;
}
void CPU::jmpIndirect()
{
    uint8_t ptrLowByte = bus->read(pc);
    uint8_t ptrHighByte = bus->read(pc + 1);
    uint16_t finalPtr = (ptrHighByte << 8) | ptrLowByte;

    uint8_t lowByte = bus->read(finalPtr);
    uint8_t highByte = bus->read(finalPtr + 1);
    if (ptrLowByte == 0xFF)
    {
        highByte = bus->read(finalPtr & 0xFF00);
    }
    uint16_t finalAddress = ((highByte << 8) | lowByte);

    pc = finalAddress;
}
void CPU::jsrAbsolute()
{

    uint8_t lowByte = bus->read(pc);
    uint8_t highByte = bus->read(pc + 1);
    uint16_t subAddress = (highByte << 8) | lowByte;

    uint16_t returnAddress = pc + 2 - 1;

    bus->write(0x0100 + sp, (returnAddress >> 8) & 0xFF);
    sp--;

    bus->write(0x0100 + sp, returnAddress & 0xFF);
    sp--;

    pc = subAddress;
}
void CPU::rts()
{
    sp++;
    uint8_t lowByte = bus->read(0x0100 + sp);
    sp++;
    uint8_t highByte = bus->read(0x0100 + sp);

    uint16_t subAddress = (highByte << 8) | lowByte;

    pc = subAddress + 1;
}
void CPU::adcImmediate()
{
    uint8_t C, V = 0;
    uint8_t operand = bus->read(pc++);
    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;
    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcZeroPage()
{
    uint8_t C, V = 0;
    uint8_t zpAddr = bus->read(pc++);
    uint8_t operand = bus->read(zpAddr);
    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;
    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcZeroPageX()
{
    uint8_t C, V = 0;
    uint8_t baseAddr = bus->read(pc++);
    uint8_t zpAddr = (baseAddr + X) & 0xFF;
    uint8_t operand = bus->read(zpAddr);
    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;
    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcAbsolute()
{
    uint8_t C, V = 0;
    uint8_t lowByte = bus->read(pc++);
    uint8_t highByte = bus->read(pc++);
    uint16_t baseAddr = (highByte << 8) | lowByte;
    uint8_t operand = bus->read(baseAddr);

    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;

    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcAbsoluteX()
{
    uint8_t C, V = 0;
    uint8_t lowByte = bus->read(pc++);
    uint8_t highByte = bus->read(pc++);
    uint16_t baseAddr = ((highByte << 8) | lowByte) + X;
    uint8_t operand = bus->read(baseAddr);

    if ((baseAddr & 0xFF00) != (lowByte << 8))
        cycles++;

    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;

    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcAbsoluteY()
{
    uint8_t C, V = 0;
    uint8_t lowByte = bus->read(pc++);
    uint8_t highByte = bus->read(pc++);
    uint16_t baseAddr = ((highByte << 8) | lowByte) + Y;
    uint8_t operand = bus->read(baseAddr);

    if ((baseAddr & 0xFF00) != (lowByte << 8))
        cycles++;

    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;

    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcIndexedIndirect()
{
    uint8_t C, V = 0;
    uint8_t zp = bus->read(pc++);
    uint8_t ptr = (zp + X) & 0xFF;
    uint8_t lowByte = bus->read(ptr);
    uint8_t highByte = bus->read((ptr + 1) & 0xFF);

    uint16_t effAddress = (highByte << 8) | lowByte;

    uint8_t operand = bus->read(effAddress);

    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;

    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
}
void CPU::adcIndirectIndexed()
{
    uint8_t C, V = 0;
    uint8_t zp = bus->read(pc++);
    uint8_t lowByte = bus->read(zp);
    uint8_t highByte = bus->read((zp + 1) & 0xFF);

    uint16_t effAddress = ((highByte << 8) | lowByte) + Y;

    if ((effAddress & 0xFF00) != (lowByte << 8))
        cycles++;

    uint8_t operand = bus->read(effAddress);

    uint16_t result = A + operand + (P & 0x01);
    uint8_t newRes = result & 0xFF;

    C = (result > 0xFF) ? 1 : 0;
    V = (~(A ^ operand) & (A ^ newRes) & 0x80) ? 1 : 0;

    setVC(V, C);

    A = newRes;

    setNZ(A);
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
