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
        table[0x00] = {"BRK", &CPU::brk, AddressingMode::IMPLIED, 7};
        table[0xEA] = {"NOP", &CPU::nop, AddressingMode::IMPLIED, 2};
        table[0x40] = {"RTI", &CPU::rti, AddressingMode::IMPLIED, 6};

        table[0xE8] = {"INX", &CPU::inx, AddressingMode::IMPLIED, 2};
        table[0xC8] = {"INY", &CPU::iny, AddressingMode::IMPLIED, 2};

        table[0xCA] = {"DEX", &CPU::dex, AddressingMode::IMPLIED, 2};
        table[0x88] = {"DEY", &CPU::dey, AddressingMode::IMPLIED, 2};

        table[0x18] = {"CLC", &CPU::clc, AddressingMode::IMPLIED, 2};
        table[0x38] = {"SEC", &CPU::sec, AddressingMode::IMPLIED, 2};

        table[0xD8] = {"CLD", &CPU::cld, AddressingMode::IMPLIED, 2};

        table[0x78] = {"SEI", &CPU::sei, AddressingMode::IMPLIED, 2};
        table[0x58] = {"CLI", &CPU::cli, AddressingMode::IMPLIED, 2};

        table[0x78] = {"SEI", &CPU::sei, AddressingMode::IMPLIED, 2};
        table[0x58] = {"CLI", &CPU::cli, AddressingMode::IMPLIED, 2};

        table[0xA9] = {"LDA", &CPU::ldaImmediate, AddressingMode::IMMEDIATE, 2};
        table[0xA5] = {"LDA", &CPU::ldaZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0xB5] = {"LDA", &CPU::ldaZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
        table[0xAD] = {"LDA", &CPU::ldaAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0xBD] = {"LDA", &CPU::ldaAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
        table[0xB9] = {"LDA", &CPU::ldaAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
        table[0xA1] = {"LDA", &CPU::ldaIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
        table[0xB1] = {"LDA", &CPU::ldaIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

        table[0x85] = {"STA", &CPU::staZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0x95] = {"STA", &CPU::staZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
        table[0x8D] = {"STA", &CPU::staAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0x9D] = {"STA", &CPU::staAbsoluteX, AddressingMode::ABSOLUTE_X, 5};
        table[0x99] = {"STA", &CPU::staAbsoluteY, AddressingMode::ABSOLUTE_Y, 5};
        table[0x81] = {"STA", &CPU::staIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
        table[0x91] = {"STA", &CPU::staIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 6};

        table[0xAA] = {"TAX", &CPU::tax, AddressingMode::IMPLIED, 2};
        table[0xA8] = {"TAY", &CPU::tay, AddressingMode::IMPLIED, 2};
        table[0x8A] = {"TXA", &CPU::txa, AddressingMode::IMPLIED, 2};
        table[0x98] = {"TYA", &CPU::tya, AddressingMode::IMPLIED, 2};

        table[0xBA] = {"TSX", &CPU::tsx, AddressingMode::IMPLIED, 2};
        table[0x9A] = {"TXS", &CPU::txs, AddressingMode::IMPLIED, 2};

        table[0x48] = {"PHA", &CPU::pha, AddressingMode::IMPLIED, 3};
        table[0x68] = {"PLA", &CPU::pla, AddressingMode::IMPLIED, 4};
        table[0x08] = {"PHP", &CPU::php, AddressingMode::IMPLIED, 3};
        table[0x28] = {"PLP", &CPU::plp, AddressingMode::IMPLIED, 4};

        table[0x4C] = {"JMP", &CPU::jmpAbsolute, AddressingMode::ABSOLUTE, 3};
        table[0x6C] = {"JMP", &CPU::jmpIndirect, AddressingMode::INDIRECT, 5};

        table[0x20] = {"JSR", &CPU::jsrAbsolute, AddressingMode::ABSOLUTE, 6};
        table[0x60] = {"RTS", &CPU::rts, AddressingMode::IMPLIED};
    };
    uint8_t dataBus, A, X, Y, P, sp, cycles, opState;
    uint16_t pc;
    bool NMI, IRQ, RESET;
    const uint32_t CLOCK_SPEED = 1'790'000;

    void reset();
    void clock();
    void connectBus(Bus *b);

    void brk();
    void nop();
    void rti();

    void inx();
    void iny();

    void dex();
    void dey();

    void clc();
    void sec();

    void cld();

    void sei();
    void cli();

    void ldaImmediate();
    void ldaZeroPage();
    void ldaZeroPageX();
    void ldaAbsolute();
    void ldaAbsoluteX();
    void ldaAbsoluteY();
    void ldaIndexedIndirect();
    void ldaIndirectIndexed();

    void staZeroPage();
    void staZeroPageX();
    void staAbsolute();
    void staAbsoluteX();
    void staAbsoluteY();
    void staIndexedIndirect();
    void staIndirectIndexed();

    void tax();
    void tay();
    void txa();
    void tya();

    void tsx();
    void txs();

    void pha();
    void pla();
    void php();
    void plp();

    void jmpAbsolute();
    void jmpIndirect();

    void jsrAbsolute();
    void rts();
};