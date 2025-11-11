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

        table[0x2C] = {"BIT", &CPU::bitAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0x24] = {"BIT", &CPU::bitZeroPage, AddressingMode::ZERO_PAGE, 3};

        table[0xEE] = {"INC", &CPU::incAbsolute, AddressingMode::ABSOLUTE, 6};
        table[0xFE] = {"INC", &CPU::incAbsoluteX, AddressingMode::ABSOLUTE_X, 7};
        table[0xE6] = {"INC", &CPU::incZeroPage, AddressingMode::ZERO_PAGE, 5};
        table[0xF6] = {"INC", &CPU::incZeroPageX, AddressingMode::ABSOLUTE, 6};

        table[0xE8] = {"INX", &CPU::inx, AddressingMode::IMPLIED, 2};
        table[0xC8] = {"INY", &CPU::iny, AddressingMode::IMPLIED, 2};

        table[0xCE] = {"DEC", &CPU::decAbsolute, AddressingMode::ABSOLUTE, 6};
        table[0xDE] = {"DEC", &CPU::decAbsoluteX, AddressingMode::ABSOLUTE_X, 7};
        table[0xC6] = {"DEC", &CPU::decZeroPage, AddressingMode::ZERO_PAGE, 5};
        table[0xD6] = {"DEC", &CPU::decZeroPageX, AddressingMode::ABSOLUTE, 6};

        table[0xCA] = {"DEX", &CPU::dex, AddressingMode::IMPLIED, 2};
        table[0x88] = {"DEY", &CPU::dey, AddressingMode::IMPLIED, 2};

        table[0x18] = {"CLC", &CPU::clc, AddressingMode::IMPLIED, 2};
        table[0x38] = {"SEC", &CPU::sec, AddressingMode::IMPLIED, 2};

        table[0x78] = {"SEI", &CPU::sei, AddressingMode::IMPLIED, 2};
        table[0x58] = {"CLI", &CPU::cli, AddressingMode::IMPLIED, 2};

        table[0xB8] = {"CLV", &CPU::clv, AddressingMode::IMPLIED, 2};

        table[0xA9] = {"LDA", &CPU::ldaImmediate, AddressingMode::IMMEDIATE, 2};
        table[0xA5] = {"LDA", &CPU::ldaZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0xB5] = {"LDA", &CPU::ldaZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
        table[0xAD] = {"LDA", &CPU::ldaAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0xBD] = {"LDA", &CPU::ldaAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
        table[0xB9] = {"LDA", &CPU::ldaAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
        table[0xA1] = {"LDA", &CPU::ldaIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
        table[0xB1] = {"LDA", &CPU::ldaIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

        table[0xA2] = {"LDX", &CPU::ldxImmediate, AddressingMode::IMMEDIATE, 2};
        table[0xA6] = {"LDX", &CPU::ldxZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0xB6] = {"LDX", &CPU::ldxZeroPageY, AddressingMode::ZERO_PAGE_Y, 4};
        table[0xAE] = {"LDX", &CPU::ldxAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0xBE] = {"LDX", &CPU::ldxAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};

        table[0xA0] = {"LDY", &CPU::ldyImmediate, AddressingMode::IMMEDIATE, 2};
        table[0xA4] = {"LDY", &CPU::ldyZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0xB4] = {"LDY", &CPU::ldyZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
        table[0xAC] = {"LDY", &CPU::ldyAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0xBC] = {"LDY", &CPU::ldyAbsoluteX, AddressingMode::ABSOLUTE_X, 4};

        table[0x85] = {"STA", &CPU::staZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0x95] = {"STA", &CPU::staZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
        table[0x8D] = {"STA", &CPU::staAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0x9D] = {"STA", &CPU::staAbsoluteX, AddressingMode::ABSOLUTE_X, 5};
        table[0x99] = {"STA", &CPU::staAbsoluteY, AddressingMode::ABSOLUTE_Y, 5};
        table[0x81] = {"STA", &CPU::staIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
        table[0x91] = {"STA", &CPU::staIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 6};

        table[0x86] = {"STX", &CPU::stxZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0x96] = {"STX", &CPU::stxZeroPageY, AddressingMode::ZERO_PAGE_Y, 4};
        table[0x8E] = {"STX", &CPU::stxAbsolute, AddressingMode::ABSOLUTE, 4};

        table[0x84] = {"STY", &CPU::styZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0x94] = {"STY", &CPU::styZeroPageX, AddressingMode::ZERO_PAGE_Y, 4};
        table[0x8C] = {"STY", &CPU::styAbsolute, AddressingMode::ABSOLUTE, 4};

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

        table[0x69] = {"ADC", &CPU::adcImmediate, AddressingMode::IMMEDIATE, 2};
        table[0x65] = {"ADC", &CPU::adcZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0x75] = {"ADC", &CPU::adcZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
        table[0x6D] = {"ADC", &CPU::adcAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0x7D] = {"ADC", &CPU::adcAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
        table[0x79] = {"ADC", &CPU::adcAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
        table[0x61] = {"ADC", &CPU::adcIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
        table[0x71] = {"ADC", &CPU::adcIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

        table[0xE9] = {"SBC", &CPU::sbcImmediate, AddressingMode::IMMEDIATE, 2};
        table[0xE5] = {"SBC", &CPU::sbcZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0xF5] = {"SBC", &CPU::sbcZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
        table[0xED] = {"SBC", &CPU::sbcAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0xFD] = {"SBC", &CPU::sbcAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
        table[0xF9] = {"SBC", &CPU::sbcAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
        table[0xE1] = {"SBC", &CPU::sbcIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
        table[0xF1] = {"SBC", &CPU::sbcIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

        table[0x90] = {"BCC", &CPU::bcc, AddressingMode::RELATIVE, 2};
        table[0xB0] = {"BCS", &CPU::bcs, AddressingMode::RELATIVE, 2};
        table[0xF0] = {"BEQ", &CPU::beq, AddressingMode::RELATIVE, 2};
        table[0xD0] = {"BNE", &CPU::bne, AddressingMode::RELATIVE, 2};
        table[0x30] = {"BMI", &CPU::bmi, AddressingMode::RELATIVE, 2};
        table[0x10] = {"BPL", &CPU::bpl, AddressingMode::RELATIVE, 2};
        table[0x50] = {"BVC", &CPU::bvc, AddressingMode::RELATIVE, 2};
        table[0x70] = {"BVS", &CPU::bvs, AddressingMode::RELATIVE, 2};

        table[0x29] = {"AND", &CPU::andImmediate, AddressingMode::IMMEDIATE, 2};
        table[0x25] = {"AND", &CPU::andZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0x35] = {"AND", &CPU::andZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
        table[0x2D] = {"AND", &CPU::andAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0x3D] = {"AND", &CPU::andAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
        table[0x39] = {"AND", &CPU::andAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
        table[0x21] = {"AND", &CPU::andIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
        table[0x31] = {"AND", &CPU::andIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

        table[0x49] = {"EOR", &CPU::eorImmediate, AddressingMode::IMMEDIATE, 2};
        table[0x45] = {"EOR", &CPU::eorZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0x55] = {"EOR", &CPU::eorZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
        table[0x4D] = {"EOR", &CPU::eorAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0x5D] = {"EOR", &CPU::eorAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
        table[0x59] = {"EOR", &CPU::eorAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
        table[0x41] = {"EOR", &CPU::eorIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
        table[0x51] = {"EOR", &CPU::eorIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

        table[0x49] = {"EOR", &CPU::eorImmediate, AddressingMode::IMMEDIATE, 2};
        table[0x45] = {"EOR", &CPU::eorZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0x55] = {"EOR", &CPU::eorZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
        table[0x4D] = {"EOR", &CPU::eorAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0x5D] = {"EOR", &CPU::eorAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
        table[0x59] = {"EOR", &CPU::eorAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
        table[0x41] = {"EOR", &CPU::eorIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
        table[0x51] = {"EOR", &CPU::eorIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

        table[0x09] = {"ORA", &CPU::oraImmediate, AddressingMode::IMMEDIATE, 2};
        table[0x05] = {"ORA", &CPU::oraZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0x15] = {"ORA", &CPU::oraZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
        table[0x0D] = {"ORA", &CPU::oraAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0x1D] = {"ORA", &CPU::oraAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
        table[0x19] = {"ORA", &CPU::oraAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
        table[0x01] = {"ORA", &CPU::oraIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
        table[0x11] = {"ORA", &CPU::oraIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

        table[0xC9] = {"CMP", &CPU::cmpImmediate, AddressingMode::IMMEDIATE, 2};
        table[0xC5] = {"CMP", &CPU::cmpZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0xD5] = {"CMP", &CPU::cmpZeroPageX, AddressingMode::ZERO_PAGE_X, 4};
        table[0xCD] = {"CMP", &CPU::cmpAbsolute, AddressingMode::ABSOLUTE, 4};
        table[0xDD] = {"CMP", &CPU::cmpAbsoluteX, AddressingMode::ABSOLUTE_X, 4};
        table[0xD9] = {"CMP", &CPU::cmpAbsoluteY, AddressingMode::ABSOLUTE_Y, 4};
        table[0xC1] = {"CMP", &CPU::cmpIndexedIndirect, AddressingMode::INDEXED_INDIRECT, 6};
        table[0xD1] = {"CMP", &CPU::cmpIndirectIndexed, AddressingMode::INDIRECT_INDEXED, 5};

        table[0xE0] = {"CPX", &CPU::cpxImmediate, AddressingMode::IMMEDIATE, 2};
        table[0xE4] = {"CPX", &CPU::cpxZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0xEC] = {"CPX", &CPU::cpxAbsolute, AddressingMode::ABSOLUTE, 4};

        table[0xC0] = {"CPY", &CPU::cpyImmediate, AddressingMode::IMMEDIATE, 2};
        table[0xC4] = {"CPY", &CPU::cpyZeroPage, AddressingMode::ZERO_PAGE, 3};
        table[0xCC] = {"CPY", &CPU::cpyAbsolute, AddressingMode::ABSOLUTE, 4};

        table[0x0A] = {"ASL", &CPU::aslAccumulator, AddressingMode::ACCUMULATOR, 2};
        table[0x06] = {"ASL", &CPU::aslZeroPage, AddressingMode::ZERO_PAGE, 5};
        table[0x16] = {"ASL", &CPU::aslZeroPageX, AddressingMode::ZERO_PAGE_X, 6};
        table[0x0E] = {"ASL", &CPU::aslAbsolute, AddressingMode::ABSOLUTE, 6};
        table[0x1E] = {"ASL", &CPU::aslAbsoluteX, AddressingMode::ABSOLUTE_X, 7};

        table[0x4A] = {"LSR", &CPU::lsrAccumulator, AddressingMode::ACCUMULATOR, 2};
        table[0x46] = {"LSR", &CPU::lsrZeroPage, AddressingMode::ZERO_PAGE, 5};
        table[0x56] = {"LSR", &CPU::lsrZeroPageX, AddressingMode::ZERO_PAGE_X, 6};
        table[0x4E] = {"LSR", &CPU::lsrAbsolute, AddressingMode::ABSOLUTE, 6};
        table[0x5E] = {"LSR", &CPU::lsrAbsoluteX, AddressingMode::ABSOLUTE_X, 7};

        table[0x6A] = {"ROR", &CPU::rorAccumulator, AddressingMode::ACCUMULATOR, 2};
        table[0x66] = {"ROR", &CPU::rorZeroPage, AddressingMode::ZERO_PAGE, 5};
        table[0x76] = {"ROR", &CPU::rorZeroPageX, AddressingMode::ZERO_PAGE_X, 6};
        table[0x6E] = {"ROR", &CPU::rorAbsolute, AddressingMode::ABSOLUTE, 6};
        table[0x7E] = {"ROR", &CPU::rorAbsoluteX, AddressingMode::ABSOLUTE_X, 7};

        table[0x2A] = {"ROL", &CPU::rolAccumulator, AddressingMode::ACCUMULATOR, 2};
        table[0x26] = {"ROL", &CPU::rolZeroPage, AddressingMode::ZERO_PAGE, 5};
        table[0x36] = {"ROL", &CPU::rolZeroPageX, AddressingMode::ZERO_PAGE_X, 6};
        table[0x2E] = {"ROL", &CPU::rolAbsolute, AddressingMode::ABSOLUTE, 6};
        table[0x3E] = {"ROL", &CPU::rolAbsoluteX, AddressingMode::ABSOLUTE_X, 7};
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

    void bitAbsolute();
    void bitZeroPage();

    void incAbsolute();
    void incAbsoluteX();
    void incZeroPage();
    void incZeroPageX();

    void decAbsolute();
    void decAbsoluteX();
    void decZeroPage();
    void decZeroPageX();

    void inx();
    void iny();

    void dex();
    void dey();

    void clc();
    void sec();

    void sei();
    void cli();

    void clv();

    void ldaImmediate();
    void ldaZeroPage();
    void ldaZeroPageX();
    void ldaAbsolute();
    void ldaAbsoluteX();
    void ldaAbsoluteY();
    void ldaIndexedIndirect();
    void ldaIndirectIndexed();

    void ldxImmediate();
    void ldxZeroPage();
    void ldxZeroPageY();
    void ldxAbsolute();
    void ldxAbsoluteY();

    void ldyImmediate();
    void ldyZeroPage();
    void ldyZeroPageX();
    void ldyAbsolute();
    void ldyAbsoluteX();

    void staZeroPage();
    void staZeroPageX();
    void staAbsolute();
    void staAbsoluteX();
    void staAbsoluteY();
    void staIndexedIndirect();
    void staIndirectIndexed();

    void stxZeroPage();
    void stxZeroPageY();
    void stxAbsolute();

    void styZeroPage();
    void styZeroPageX();
    void styAbsolute();

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

    void adcImmediate();
    void adcZeroPage();
    void adcZeroPageX();
    void adcAbsolute();
    void adcAbsoluteX();
    void adcAbsoluteY();
    void adcIndexedIndirect();
    void adcIndirectIndexed();

    void sbcImmediate();
    void sbcZeroPage();
    void sbcZeroPageX();
    void sbcAbsolute();
    void sbcAbsoluteX();
    void sbcAbsoluteY();
    void sbcIndexedIndirect();
    void sbcIndirectIndexed();

    void bcc();
    void bcs();
    void beq();
    void bne();
    void bmi();
    void bpl();
    void bvc();
    void bvs();

    void andImmediate();
    void andZeroPage();
    void andZeroPageX();
    void andAbsolute();
    void andAbsoluteX();
    void andAbsoluteY();
    void andIndexedIndirect();
    void andIndirectIndexed();

    void eorImmediate();
    void eorZeroPage();
    void eorZeroPageX();
    void eorAbsolute();
    void eorAbsoluteX();
    void eorAbsoluteY();
    void eorIndexedIndirect();
    void eorIndirectIndexed();

    void oraImmediate();
    void oraZeroPage();
    void oraZeroPageX();
    void oraAbsolute();
    void oraAbsoluteX();
    void oraAbsoluteY();
    void oraIndexedIndirect();
    void oraIndirectIndexed();

    void cmpImmediate();
    void cmpZeroPage();
    void cmpZeroPageX();
    void cmpAbsolute();
    void cmpAbsoluteX();
    void cmpAbsoluteY();
    void cmpIndexedIndirect();
    void cmpIndirectIndexed();

    void cpxImmediate();
    void cpxZeroPage();
    void cpxAbsolute();

    void cpyImmediate();
    void cpyZeroPage();
    void cpyAbsolute();

    void aslAccumulator();
    void aslZeroPage();
    void aslZeroPageX();
    void aslAbsolute();
    void aslAbsoluteX();

    void lsrAccumulator();
    void lsrZeroPage();
    void lsrZeroPageX();
    void lsrAbsolute();
    void lsrAbsoluteX();

    void rorAccumulator();
    void rorZeroPage();
    void rorZeroPageX();
    void rorAbsolute();
    void rorAbsoluteX();

    void rolAccumulator();
    void rolZeroPage();
    void rolZeroPageX();
    void rolAbsolute();
    void rolAbsoluteX();

    void setNZ(uint8_t reg);
    void setVC(uint8_t V, uint8_t C);
    void compare(uint8_t reg, uint8_t value);
};