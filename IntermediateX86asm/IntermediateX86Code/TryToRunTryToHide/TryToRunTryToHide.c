//TryToRunTryToHide.c: Userspace component to print values before transitioning to kernel
//in BreakOnThruToTheOtherSide.c (which prints the same values)
//Instructions Learned: int 3
//Written by Xeno Kovah, Jan. 15th 2010
//Originally for use with MITRE Institute Intermediate x86 Class
//Licensed under Creative Commons Share Alike license.
//full details available here: http://creativecommons.org/licenses/by-sa/3.0
//P.s. Don't comment on the efficency or style of the code, it's tutorial code! :P

#include <stdio.h>

//You can change this to invoke other Interrupt/Trap gate entries
//in the IDT (NOTE!: Kernel component will not work with task gates! You've been warned!)
#define HOOKED_HANDLER_INDEX 0xEE

int main(){
	unsigned short myCS, myDS, myES, myFS, myGS, mySS, myTR;
	unsigned int myEAX, myEBX, myECX, myEDX, myEDI, myESI, myEFLAGS, myESP, myEBP;

	//Move the registers into the C variables
	__asm{
		mov myCS, cs;
		mov myDS, ds;
		mov myES, es;
		mov myFS, fs;
		mov myGS, gs;
		mov mySS, ss;
		mov myEAX, eax;
		mov myEBX, ebx;
		mov myECX, ecx;
		mov myEDX, edx;
		mov myEDI, edi;
		mov myESI, esi;
		mov myEBP, ebp;
		mov myESP, esp;
		push eax; //save eax because we're going to clobber it
		pushfd; //Push eflags
		mov eax, [esp];//can't do memory to memory, so move to reg first
						//just chose eax arbitrarily
		mov myEFLAGS, eax;
		popfd; //compensate for the pushfd
		str ax; //store the task register
		mov myTR, ax;
		pop eax; //restore eax because we're done clobbering it	}
	}
	//Note: putting the paramters on the stack for the following printf()s
	//clobbers some registers, so we want to save them now and restore them later
	//so that what's printed out is accurate to what the values are pre-interrupt
	__asm{
		pushad;
	};
	
	printf("tr = %#x\n", myTR);
	printf("cs = %#x\n", myCS);
	printf("ss = %#x\n", mySS);
	printf("ds = %#x\n", myDS);
	printf("es = %#x\n", myES);
	printf("fs = %#x\n", myFS);
	printf("gs = %#x\n", myGS);
	printf("eax = %#x\n", myEAX);
	printf("ebx = %#x\n", myEBX);
	printf("ecx = %#x\n", myECX);
	printf("edx = %#x\n", myEDX);
	printf("edi = %#x\n", myEDI);
	printf("esi = %#x\n", myESI);
	printf("ebp = %#x\n", myEBP);
	printf("esp = %#x\n", myESP);
	printf("eflags = %#x\n", myEFLAGS);

	//Parse the eflags per Vol 3a, Sect 2.3
	//note: the calculations here alter the registers
//	printf("EFLAGS: Trap Flag = %d\n", (myEFLAGS >> 8) & 0x1);
	printf("EFLAGS: Interrupt Flag = %d\n", (myEFLAGS >> 9) & 0x1);
//	printf("EFLAGS: IOPL = %d\n", (myEFLAGS >> 12) & 0x3);
//	printf("EFLAGS: Resume Flag = %d\n", (myEFLAGS >> 16) & 0x1);

	printf("\n\nTry to run, try to hide...\n\n");

	//Restore the registers which we saved before, so that the values
	//before the interrupt are accurate to what was printed out
	__asm{
		popad;
	};

	__asm nop;
	__asm{int HOOKED_HANDLER_INDEX};
	return 0xf00d;
}