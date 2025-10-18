#pragma once
#include <cstdint>

class Bus;

class CPU
{

public:
    Bus *bus = nullptr;
    CPU() = default;
    uint8_t dataBus, A, X, Y, P, sp, cycles, opState;
    uint16_t pc;
    bool NMI, IRQ, RESET;
    const uint32_t CLOCK_SPEED = 1'790'000;

    void reset();
    void clock();
    void connectBus(Bus *b);
};