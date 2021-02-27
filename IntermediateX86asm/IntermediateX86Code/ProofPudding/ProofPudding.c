//ProofPudding.c: A program which reads its own memory in order to confirm 
//that when a breakpoint is set, it overwrites a byte with the 0xCC form of INT 3
//Written by Xeno Kovah, Jan. 16th 2010
//Originally for use with MITRE Institute Intermediate x86 Class
//Licensed under Creative Commons Share Alike license.
//full details available here: http://creativecommons.org/licenses/by-sa/3.0
//P.s. Don't comment on the efficency or style of the code, it's tutorial code! :P

#include <stdio.h>

int main(){
	unsigned char * pointer;

	//Call relative 32 bits. I'm calling to an address at EIP + 0 bytes
	//So this will redirect control flow to the instruction after the call
	//(which is what would have happened anyway). BUT the key point is that
	//remember that a call instruction pushes the address of the instruction
	//after it onto the stack. Thus, the net effect of "call 0" is "push eip"! :)
	//Oh, and obviously I did it this way because the assembler wouldn't
	//let me write "call 0"
	__asm _emit 0xE8;
	__asm _emit 0;
	__asm _emit 0;
	__asm _emit 0;
	__asm _emit 0;

	//This instruction pops its own address off of the stack and into eax
	//(because the call 0 put the address of this instruction onto the stack)
	//		(For lab purposes you should have a breakpoint on the pop, and then when
	//		you break there, switch to disassembly viewing mode, and note that it 
	//		does *not* show 0xCC embedded in the binary)
	__asm pop eax; //pop eax = 0x58
	//The net effect of "call 0" + "pop eax" = "mov eax, eip" 
	//(which is not allowed with a normal mov instruction) :)

	//Store the address of the 
	__asm mov pointer, eax; 

	printf("The proof is in the pudding!\n");
	printf("The proof is in the pointer!: %#x\n", *pointer);
	printf("Ipso facto, the pointer is the pudding!\n");

	return 0xfeeb1e;
}