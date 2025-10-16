#include <iostream>
#include <cstdint>

int main()
{
    class CPU
    {
    public:
        uint16_t bus;
        uint8_t dataBus;
        uint8_t A;
        uint8_t X;
        uint8_t Y;
        uint8_t P;
        uint16_t pc;
        uint8_t sp;
        uint8_t cycles;
        uint8_t opState;
        bool NMI;
        bool IRQ;
        bool RESET;
        const uint32_t CLOCK_SPEED = 1'790'000;

        void initialize()
        {
            bus = 0;
            dataBus = 0;
            A = 0;
            X = 0;
            Y = 0;
            P = 0x24;
            pc = 0;
            sp = 0xFD;
            cycles = 0;
            opState = 0;
            NMI = false;
            IRQ = false;
            RESET = false;
        }
    };
}