#pragma once

void PSPPatcher_Init(int base);
unsigned int GetBaseAddr();
void PatchInt(unsigned int addr, unsigned int value);
void PatchShort(unsigned int addr, unsigned short value);
void PatchChar(unsigned int addr, unsigned char value);
void MakeCall(unsigned int addr, unsigned int func);
void MakeJump(unsigned int addr, unsigned int func);


void MakeLUI(unsigned int addr, unsigned int value);
void MakeADDIU(unsigned int addr, unsigned int value);
void Nop(unsigned int addr);
int    ReadInt(unsigned int addr);
short  ReadShort(unsigned int addr);
char   ReadChar(unsigned int addr);
