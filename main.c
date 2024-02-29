#include <pspsdk.h>
#include <pspkernel.h>

#include <stdio.h>
#include <string.h>
#include <pspctrl.h>
#include <systemctrl.h>
#include "pspmem.h"


#define TARGET_MODULE "Model"
#define MODULE_NAME "nipvp"

PSP_MODULE_INFO(MODULE_NAME, 0x1007, 1, 0);

static STMOD_HANDLER previous;
SceUID thid;
u32 mod_text_addr;

u32 RegionAddr = 0x08800000;
u32 Spawnpoint = 0x08800000;
u32 MapCode = 0x08800000;
u32 MissonCode = 0x08800000;
int Coop = 0;

int GetGameVersion()
{
	if (ReadChar(0x101B6B) == 0)
		return 0;
	else if (ReadChar(0x101B6B) != 0)
		return 1;
	else
		return -1;
}

int ptrx(u32 addr, u8 d)
{
	u32 ptr = 0x08800000;
	
	if (ReadInt(addr)>= 0x08800000) {
		ptr = ReadInt(addr);
		return ReadInt(ptr + d - 0x08800000);
	}
	else{
		return 0;
	}

}

int module_thread()
{
	while (1)
	{
		
		if (GetGameVersion() == 0) {

			if (ReadChar(0x405800) == 0x2) {
				Coop = 1;
			}
			else if (ReadChar(0x405800) != 0x2){
				Coop = 0;
			}
		}
		else if (GetGameVersion() == 1) {

			if (ReadChar(0x4069C0) == 0x2) {
				Coop = 1;
			}
			else if (ReadChar(0x4069C0) != 0x2){
				Coop = 0;
			}
		}

		if (Coop == 1) {

			//NPJH50435
			if (GetGameVersion() == 0){

				//Balance
				PatchInt(0x95914,0x34040150);
				PatchInt(0x9592C,0x34040080);
				//Remove enemies
				PatchInt(0x101238,0x1000006B);
				//2P Health Bar
				Nop(0x88A7C);
				Nop(0x8D3AC);
				Nop(0x8D544);
				//Better Ending
				Nop(0x138E00);
				PatchInt(0x138E10,0x34040034);
				PatchInt(0x139050,0x3404000C);
				// //Skip Opening
				Nop(0xFD920);
				PatchInt(0x1019f4,0x10000004);
				PatchInt(0x101A1C,0x34020000);

			}

			//ULUS10582
			if (GetGameVersion() == 1){

				//Balance
				PatchInt(0x96244,0x34040150);
				PatchInt(0x9625C,0x34040080);
				//Remove enemies
				PatchInt(0x101B68,0x1000006B);
				//2P Health Bar
				Nop(0x893AC);
				Nop(0x8DCDC);
				Nop(0x8DE74);
				//Better Ending
				Nop(0x139730);
				PatchInt(0x139740,0x34040034);
				PatchInt(0x139980,0x3404000C);
				//Skip Opening
				Nop(0xFE250);
				PatchInt(0x102324,0x10000004);
				PatchInt(0x10234C,0x34020000);

			}
			
			//Fix spawnpoint one by one...	
			switch (ReadShort(MissonCode)){

				case 0xA6: case 0x9C: 
				PatchInt(Spawnpoint,0x34050001);
				break;

				case 0xA3:
				PatchInt(Spawnpoint,0x34050001);
				PatchInt(MissonCode + 0x4,0x96);
				PatchInt(MapCode,0x3404002D);
				break;
				
				case 0xAA:
				PatchInt(Spawnpoint,0x3405000A);
				break;

				case 0xA2: case 0x96:
				PatchInt(Spawnpoint,0x34050003);
				break;

				case 0xA5:
				PatchInt(Spawnpoint,0x34050004);
				break;

				case 0x98:
				PatchInt(Spawnpoint,0x34050011);
				break;

				default:
				PatchInt(Spawnpoint,0x95250006);
				PatchInt(MapCode,0x96240004);
				// PatchInt(MissonCode + 0x4,ReadShort(MissonCode));

				//imperfect : case 0x95: case 0x99: case 0x9D: case 0x9B: case 0xA0: case 0x9F: case 0xA1: case 0xAD: case 0xA4: case 0x97: case 0xAC: 

			}



			if (ReadInt(RegionAddr) >= 0x08800000) {

				//1P
				PatchInt(0x2000,ReadInt(RegionAddr));
				PatchInt(0x2004,ptrx(0x2000,0x4));
				PatchInt(0x2008,ptrx(0x2004,0x0));
				PatchInt(0x200C,ptrx(0x2008,0x0));
				PatchInt(0x2010,ptrx(0x200C,0x90));
				PatchInt(0x2014,ptrx(0x2010,0x0));

				//2P
				PatchInt(0x2020,ReadInt(0x2008));
				PatchInt(0x2024,ptrx(0x2020,0x4));
				PatchInt(0x2028,ptrx(0x2024,0x90));
				PatchInt(0x202C,ptrx(0x2028,0x0));

				if (ReadInt(0x200C) >= 0x08800000 && ReadInt(0x2014) >= 0x08800000){
				
					//PVP
					PatchChar(ReadInt(0x200C) - 0x08800000 + 0xA4,0x2);
					PatchChar(ReadInt(0x2014) - 0x08800000 + 0x8,0x2);
					PatchChar(ReadInt(0x2014) - 0x08800000 + 0x538,0x2);

					//Balance
					PatchShort(ReadInt(0x2014) - 0x08800000 + 0x7D0,0x150);
					PatchShort(ReadInt(0x2014) - 0x08800000 + 0x7E0,0x80);

				}


				if (ReadInt(0x202C) >= 0x08800000){

					//Balance
					PatchShort(ReadInt(0x202C) - 0x08800000 + 0x7D0,0x150);
					PatchShort(ReadInt(0x202C) - 0x08800000 + 0x7E0,0x80);
					
				}

			}
		}
		if (Coop == 0) {
			if (GetGameVersion() == 0) {

				PatchInt(0x95914,0x8E24001C);
				PatchInt(0x9592C,0x8E24002C);

				PatchInt(0x101238,0x5040006B);

				PatchInt(0x88A7C,0x10A0004E);
				PatchInt(0x8D3AC,0X9204005D);
				PatchInt(0x8D544,0X10800059);

				PatchInt(0x138E00,0x02608025);
				PatchInt(0x138E10,0x3404000D);
				PatchInt(0x139050,0x3404000D);

				PatchInt(0xFD920,0x0E21D62D);
				PatchInt(0x1019f4,0x12200004);
				PatchInt(0x101A1C,0x34020001);

			}
			else if (GetGameVersion() == 1) {

				PatchInt(0x96244,0x8E24001C);
				PatchInt(0x9625C,0x8E24002C);

				PatchInt(0x101B68,0x5040006B);

				PatchInt(0x893AC,0x10A0004E);
				PatchInt(0x8DCDC,0X9204005D);
				PatchInt(0x8DE74,0X10800059);

				PatchInt(0x139730,0x02608025);
				PatchInt(0x139740,0x10800006);
				PatchInt(0x139980,0x3404000D);
				
				PatchInt(0xFE250,0x0E21D62D);
				PatchInt(0x102324,0x12200004);
				PatchInt(0x10234C,0x34020001);
			}
		}
		if(ReadInt(RegionAddr) < 0x08800000){
			sceKernelDelayThread(3000000);
			for (u32 i = 0x2000; i <= 0x202C; i += 0x4){
				Nop(i);
			}
		}

		sceKernelDelayThread(100000);	
		sceKernelDcacheWritebackAll();
		sceKernelIcacheClearAll();

	}
	sceKernelExitDeleteThread(0);
	return 0;
}

int Init(unsigned int addr)
{
	
	PSPPatcher_Init(addr);

	//NPJH50435
	if (GetGameVersion() == 0){
		
		RegionAddr = 0x3D898C;
		Spawnpoint = 0xFFA30;
		MissonCode = 0x405884;
		MapCode = 0xFC00C;

		//Remove Circles
		PatchInt(0x80808,0x1000001A);
	}
	//ULUS10582
	if (GetGameVersion() == 1){

		RegionAddr = 0x3D9B54;
		Spawnpoint = 0x100360;
		MissonCode = 0x406A44;
		MapCode = 0xFC93C;

		//Remove Circles
		PatchInt(0x81138,0x1000001A);

	}


	sceKernelStartThread(thid, 0, 0);

	return 0;
}

int OnModuleStart(SceModule2 *mod) {
	char *modname = mod->modname;

	if (strcmp(modname, "Model") == 0) {
		mod_text_addr = mod->text_addr;

		Init(0x08800000);

		sceKernelDcacheWritebackAll();
	}
	if (!previous)
		return 0;

	return previous(mod);
}

int module_start(SceSize args, void *argp)
{	
 	sceKernelDelayThread(100000);
	thid = sceKernelCreateThread("nipvp", module_thread, 0x18, 2048, 0, NULL);
	if (sceIoDevctl("kemulator:", 0x00000003, NULL, 0, NULL, 0) == 0) {
		SceUID modules[10];
		int count = 0;
		if (sceKernelGetModuleIdList(modules, sizeof(modules), &count) >= 0) {
			int i;
			SceKernelModuleInfo info;
			for (i = 0; i < count; ++i) {
				info.size = sizeof(SceKernelModuleInfo);
				if (sceKernelQueryModuleInfo(modules[i], &info) < 0) {
					continue;
				}

				if (strcmp(info.name, TARGET_MODULE) == 0)
					Init(0x08800000);
			}
		}
	}
	else {
		previous = sctrlHENSetStartModuleHandler(OnModuleStart); // PSP
	}
	return 0;
}



