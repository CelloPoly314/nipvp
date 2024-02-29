#include "pspmem.h"
#include <pspsdk.h>
#include <pspkernel.h>


int pBaseAddress;

void PSPPatcher_Init(int base)
{
	pBaseAddress = base;
}

unsigned int GetBaseAddr()
{
	return pBaseAddress;
}

void PatchInt(unsigned int addr, unsigned int value)
{
	_sw(value, pBaseAddress + addr);
}

void PatchShort(unsigned int addr, unsigned short value)
{
	_sh(value, pBaseAddress + addr);
}

void PatchChar(unsigned int addr, unsigned char value)
{
	_sb(value, pBaseAddress + addr);
}

void MakeCall(unsigned int addr, unsigned int func)
{
	_sw(0x0C000000 | (((unsigned int)(func) >> 2) ), pBaseAddress + addr);
}

void MakeJump(unsigned int addr, unsigned int func)
{
	 _sw(0x08000000 | (((unsigned int)(func) & 0x0FFFFFFC) >> 2), pBaseAddress + addr);
}

void MakeLUI(unsigned int addr, unsigned int value)
{
	_sw(0x3C000000 | ((unsigned int)(value) >> 16), pBaseAddress + addr);
}

void MakeADDIU(unsigned int addr, unsigned int value)
{
	_sw(0x24000000 | ((unsigned int)(value)), pBaseAddress + addr);
}

void Nop(unsigned int addr)
{
	PatchInt(addr, 0);
}

int ReadInt(unsigned int addr)
{
	return _lw(pBaseAddress + addr);
}

short ReadShort(unsigned int addr)
{
	return _lh(pBaseAddress + addr);
}

char ReadChar(unsigned int addr)
{
	return _lb(pBaseAddress + addr);
}
