//UserspaceSegmentRegisters.c: Code to interpret userspace segment selectors
//Instructions Learned: MOV from Segment Registers to memory
//Written by Xeno Kovah, Jan. 9th 2010
//Originally for use with MITRE Institute Intermediate x86 Class
//Licensed under Creative Commons Share Alike license.
//full details available here: http://creativecommons.org/licenses/by-sa/3.0
//P.s. Don't comment on the efficency or style of the code, it's tutorial code! :P

#include <stdio.h>

//prototype for helper function
void SelectorPrint(char * segRegName, unsigned short segReg);

int main(){
	unsigned short myCS, myDS, myES, myFS, myGS, mySS;

	//Move the segment registers into the C variables
	__asm{
		mov myCS, cs;
		mov myDS, ds;
		mov myES, es;
		mov myFS, fs;
		mov myGS, gs;
		mov mySS, ss;
	}

	//Each segment register holds a 16 bit segment selector which is defined as
	//15     3 2 1 0
	//---------------
	//| Index |T|RPL| ... ASCII "ART" FTW!
	//---------------
	//RPL = Requested Privilege Level (2 bits)
	//T = Table Indicator, 0 = GDT, 1 = LDT (1 bit)
	//Index = This selector selects the Index-th entry in the GDT or LDT (13 bits)

	//Parse the meaning of the selectors stored in the registers
	SelectorPrint("cs", myCS);
	SelectorPrint("ss", mySS);
	SelectorPrint("ds", myDS);
	SelectorPrint("es", myES);
	SelectorPrint("fs", myFS);
	SelectorPrint("gs", myGS);

	return 0x0ddba11;

}

void SelectorPrint(char * segRegName, unsigned short segReg){
	printf("The segment selector stored in the %s register = %#x\n", segRegName, segReg);
	if(segReg == 0){
		printf("A segment selector of 0 is invalid because the 0th entry of the GDT is never used\n\n");
		return;
	}
	printf("\tIts Requested Privilege Level is %u\n", (segReg & 0x3));
	printf("\tIt selects a segment in the ");
	if((segReg & 0x4) == 0){
		printf("GDT ");
	}
	else{
		printf("LDT");
	}
	printf(" of index %#x\n\n", segReg >> 3);
}