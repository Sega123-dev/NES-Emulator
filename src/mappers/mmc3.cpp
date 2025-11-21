#include "mmc3.hpp"
#include <iostream>
#include <cstdint>
#include <vector>

void MMC3::reset()
{
    bankSelect = 0;
    prgMode = 0;
    chrMode = 0;
    mirroring = Mirroring::HORIZONTAL;
    wramEnabled = false;
    wramWriteProtected = false;
    irqCounter = 0;
    irqReloadValue = 0;
    irqEnabled = false;
    irqReloadFlag = false;
    prevA12 = 0;
}
MMC3::MMC3(std::vector<uint8_t> &prgData, std::vector<uint8_t> &chrData)
{
    prg = prgData;
    chr = chrData;

    wram.resize(0x2000, 0);
    reset();
}
void MMC3::updatePRGBanks()
{
    int lastBank = (prg.size() / 0x2000) - 1;
    int secondLastBank = lastBank - 1;

    if (prgMode == 0)
    {
        prgBankMap[0] = bankRegister[6];
        prgBankMap[1] = secondLastBank;
        prgBankMap[2] = bankRegister[7];
        prgBankMap[3] = lastBank;
    }
    else if (prgMode == 1)
    {
        prgBankMap[0] = secondLastBank;
        prgBankMap[1] = bankRegister[7];
        prgBankMap[2] = bankRegister[6];
        prgBankMap[3] = lastBank;
    }
}
void MMC3::updateCHRBanks()
{
    if (chrMode == 0)
    {
        chrBankMap[0] = bankRegister[0];
        chrBankMap[1] = bankRegister[0];
        chrBankMap[2] = bankRegister[1];
        chrBankMap[3] = bankRegister[1];
        chrBankMap[4] = bankRegister[2];
        chrBankMap[5] = bankRegister[3];
        chrBankMap[6] = bankRegister[4];
        chrBankMap[7] = bankRegister[5];
    }
    else if (chrMode == 1)
    {
        chrBankMap[0] = bankRegister[2];
        chrBankMap[1] = bankRegister[3];
        chrBankMap[2] = bankRegister[4];
        chrBankMap[3] = bankRegister[5];
        chrBankMap[4] = bankRegister[0];
        chrBankMap[5] = bankRegister[0];
        chrBankMap[6] = bankRegister[1];
        chrBankMap[7] = bankRegister[1];
    }
}
void MMC3::clockIRQ(int currentA12)
{
    if (prevA12 == 0 && currentA12 != 0)
    {
        if (irqReloadFlag)
        {
            irqCounter = irqReloadValue;
            irqReloadFlag = false;
        }
        else
            irqCounter--;
        if (irqEnabled && irqCounter == 0)
            ; // skip
    }
    prevA12 = currentA12;
}