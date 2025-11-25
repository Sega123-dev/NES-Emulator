#pragma once
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

class Mapper;

void dumpROM(const char *filename);
Mapper *chooseMapper();

__uint128_t header[16];
int prgRomSize; // 16KB bank
int chrRomSize; // 8KB bank
uint8_t flag6;
uint8_t flag7;
uint16_t mapperID;

bool verticalMirroring; // 0 - horizontal 1 - vertical
bool SRAM;
bool fourScreenMirroring;

std::vector<uint8_t> PRG_ROM;
std::vector<uint8_t> CHR_ROM;