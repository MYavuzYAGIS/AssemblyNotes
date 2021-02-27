//CPUID.c: Code to play with CPUID instruction
//Instructions Learned: CPUID, popfd, pushfd
//Written by Xeno Kovah, Jan. 3rd 2010
//Originally for use with MITRE Institute Intermediate x86 Class
//Licensed under Creative Commons Share Alike license.
//full details available here: http://creativecommons.org/licenses/by-sa/3.0
//P.s. Don't comment on the efficency or style of the code, it's tutorial code! :P

#include <stdio.h>

int main(){
	char someBuffer[16];
	char someOtherBuffer[16];
	char * someBufferPtr = &(someBuffer[0]);
	char * someOtherBufferPtr = &(someOtherBuffer[0]);
	//Don't care that they're uninitialized, just using them for their pointers

	/*LOCK PREFIX - 0xF0*/
	__asm mov eax, someBufferPtr;
	//Simple exchange
	__asm xchg [eax], ebx;
	//Atomic xchg
	__asm lock xchg [eax], ebx;
	//Funny. Visual Studio seems to always write the register to the left ("destination")

	/*REP PREFIX - 0xF3*/
	//Initialize source and destination
	__asm{
		mov esi, someBufferPtr;
		mov edi, someOtherBufferPtr;
	};
	__asm movs; //It just assumes the es:[edi], ds:[esi] since there's no other form
	__asm movs es:[edi], ds:[esi]; //also can specify it this way of course

	__asm mov ecx, 1; //Set the counter
	__asm rep movs;

	/*REPNE PREFIX - 0xF2*/
	__asm{
		mov esi, someBufferPtr;
		mov edi, someOtherBufferPtr;
		mov ecx, 2;
	};
	__asm cmps [edi], [esi]; //Compare byte until ZF = 0

	__asm repne cmps [edi], [esi]; //Repeat the comparison until ZF = 1 or ECX = 0

	/*SEGMENT OVERRIDE PREFIXES - CS=0x2E, SS=0x36, DS=0x3E, ES=0x26, FS=0x64, GS=0x65*/
	__asm {
		mov eax, someBufferPtr;
		mov ebx, [eax];
		mov ebx, ss:[eax];
		mov ebx, ds:[eax];
		mov ebx, cs:[eax];
	}

	/*BRANCH TAKEN HINT PREFIX - 0x3E (when used with Jccs)*/
	__asm{ jnz 0}; //Just jumps to the next instruction

	__asm _emit 0x3E; //Manually put the prefix (since I can't generate it with the mnemonic it shows)
	__asm{ jnz 0};

	/*BRANCH NOT TAKEN HINT - 0x2E (when used with Jccs)*/
	__asm _emit 0x2E; //Manually put the prefix (since I can't generate it with the mnemonic it shows)
	__asm{ jnz 0};

	/*OPERAND SIZE OVERRIDE PREFIX - 0x66*/
	//32 bit immediate operand
	__asm add eax, 0x12345678; //32 bit form

	//16 bit immediate operand
	__asm _emit 0x66;
	__asm add eax, 0x12345678; //16 bit form (instruction uses 0x5678, but emitted the full 4 bytes,
								//leaving 2 bytes - 0x34 0x12 - to be interpreted as an instruction
								//which just happens to be an XOR :)

	/*ADDRESS SIZE OVERRIDE PREFIX - 0x67*/
	__asm lea eax, [eax];//32 bit address

	__asm _emit 0x67;
	__asm lea eax, [eax];//16 bit address

	//Double whammy: override to 16 bit operand AND address size
	__asm _emit 0x66;
	__asm _emit 0x67;
	__asm lea eax, [ebx];//16 bit address


	return 0;
}