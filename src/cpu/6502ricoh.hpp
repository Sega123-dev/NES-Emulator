#pragma once
#include <cstdint>
#include <iostream>

enum AddressingMode
{
    IMPLIED,
    ACCUMULATOR,
    IMMEDIATE,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    ABSOLUTE,
    RELATIVE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    INDIRECT,
    INDEXED_INDIRECT,
    INDIRECT_INDEXED
};

class CPU;
class Bus;

struct Instruction
{
    std::string name;
    void (CPU::*execute)();
    AddressingMode mode;
    int cycles;
};

Instruction table[256];

class CPU
{

public:
    Bus *bus = nullptr;
    Instruction table[256];
    CPU()
    {
        table[0xA9] = {"LDA", &CPU::ldaImmediate, AddressingMode::IMMEDIATE, 2};
        table[0xA5] = {"LDA", &CPU::ldaZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0xB5] = {"LDA", &CPU::ldaZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
        table[0xAD] = {"LDA", &CPU::ldaAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0xBD] = {"LDA", &CPU::ldaAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
        table[0xB9] = {"LDA", &CPU::ldaAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
        table[0xA1] = {"LDA", &CPU::ldaIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
        table[0xB1] = {"LDA", &CPU::ldaIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};
    };
    uint8_t dataBus, A, X, Y, P, sp, cycles, opState;
    uint16_t pc;
    bool NMI, IRQ, RESET;
    const uint32_t CLOCK_SPEED = 1'790'000;

    void reset();
    void clock();
    void connectBus(Bus *b);

    void ldaImmediate();
    void ldaZeroPage();
    void ldaZeroPageX();
    void ldaAbsolute();
    void ldaAbsoluteX();
    void ldaAbsoluteY();
    void ldaIndexedIndirect();
    void ldaIndirectIndexed();
};