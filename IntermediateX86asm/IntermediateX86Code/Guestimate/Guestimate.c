//Guestimate.c: Code to play with RDTSC instruction
//Instructions Learned: RDTSC
//Written by Xeno Kovah, Jan. 12th 2010
//Originally for use with MITRE Institute Intermediate x86 Class
//Licensed under Creative Commons Share Alike license.
//full details available here: http://creativecommons.org/licenses/by-sa/3.0
//P.s. Don't comment on the efficency or style of the code, it's tutorial code! :P

#include <stdio.h>

int main(){
	unsigned int upperStart, lowerStart, upperEnd, lowerEnd;
	unsigned int upperDiff, lowerDiff;

	__asm{
		rdtsc;
		mov upperStart, edx;
		mov lowerStart, eax;
	}
	//PUT CODE HERE YOU WANT TO PROFILE

	//Examples: simple move, cpuid, race shift vs multiply, calls with or without frame pointers, etc
	__asm{
		mov eax, 1;
		cpuid;
	}

	//END CODE YOU WANT TO PROFILE

	__asm{
		rdtsc;
		mov upperEnd, edx;
		mov lowerEnd, eax;
	}
	printf("Start time upperStart:lowerStart = 0x%08x%08x\n", upperStart,lowerStart);
	printf("End time upperEnd:lowerEnd = 0x%08x%08x\n", upperEnd,lowerEnd);
	upperDiff = upperEnd - upperStart;
	lowerDiff = lowerEnd - lowerStart;
	printf("The profiled code took 0x%08x%08x cycles to complete\n",upperDiff,lowerDiff);
	
	return 0xca5cade;
}