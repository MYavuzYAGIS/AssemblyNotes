//CPUID.c: Code to play with CPUID instruction
//Instructions Learned: CPUID, popfd, pushfd
//Written by Xeno Kovah, Jan. 3rd 2010
//Originally for use with MITRE Institute Intermediate x86 Class
//Licensed under Creative Commons Share Alike license.
//full details available here: http://creativecommons.org/licenses/by-sa/3.0
//P.s. Don't comment on the efficency or style of the code, it's tutorial code! :P

#include <stdio.h>


//Attemps to set the ID flag in the eflags register
//Returns 1 on success, 0 on failure
//The __declspec( naked ) forces the compiler to generate no prolog/epilog
__declspec( naked ) int SetIDFlag(){
	__asm{
		pushfd;				//Get the current eflags by pushing it
							//onto the stack
		mov esi, [esp];		//Moving to a holder register because there's
							//no 'or [esp], 0x00200000' form of OR
							//esi was chosen arbitrarily
		or esi, 0x00200000;	//Set the flag by ORing with 0x00200000
		mov [esp], esi;		//Putting the value back on the stack
		popfd;				//Pop the value back into eflags
		pushfd;				//Pushes eflags onto the stack
		mov edi, [esp];		//Again, storing to a temp register
		cmp edi, esi;		//Remember cmp is like sub, so if 
							//what comes back (edi) after setting 
							//eflags is the same as what went in(esi)
							//then it would be like edi - esi = 0, 
							//and should thus set the ZeroFlag(ZF)
		mov eax, 1;			//Assume it succeeded and set the return to 1
							//NOTE: this MOV doesn't effect ZF
		jz done;			//If it succeeded and set ZF, we will skip
							//to being done, instead of executing the next 
							//instruction which sets the return to 0
		mov eax, 0;			//If we got here, we failed to set the ID flag
done:
		add esp, 4;			//Cleanup the extra value put ont
							//the stack by the pushfd
		ret;				//Exit the function
	};
}

//Attemps to clear the ID flag in the eflags register
//Returns 1 on success, 0 on failure
//The __declspec( naked ) forces the compiler to generate no prolog/epilog
__declspec( naked ) int ClearIDFlag(){
	__asm{
		pushfd;				//Get the current eflags by pushing it
							//onto the stack
		mov esi, [esp];		//Moving to a holder register because there's
							//no 'or [esp], 0x00200000' form of OR
							//esi was chosen arbitrarily
		and esi, 0xFFDFFFFF;//Clear the flag by ANDing with 0xFFDFFFFF
							//which is ~0x00200000
		mov [esp], esi;		//Putting the value back on the stack
		popfd;				//Pop the value back into eflags
		pushfd;				//Pushes eflags onto the stack
		mov edi, [esp];		//Again, storing to a temp register
		cmp edi, esi;		//Remember cmp is like sub, so if 
							//what comes back (edi) after setting 
							//eflags is the same as what went in(esi)
							//then it would be like edi - esi = 0, 
							//and should thus set the ZeroFlag(ZF)
		mov eax, 1;			//Assume it succeeded and set the return to 1
							//NOTE: this MOV doesn't effect ZF
		jz done;			//If it succeeded and set ZF, we will skip
							//to being done, instead of executing the next 
							//instruction which sets the return to 0
		mov eax, 0;			//If we got here, we failed to set the ID flag
done:
		add esp, 4;			//Cleanup the extra value put ont
							//the stack by the pushfd
		ret;				//Exit the function
	};
}

//Returns 1 on success, 0 on failure
int CheckIfWeCanUseCPUID(){
	//Local variables where we will keep copies of the eflags register
	unsigned int myEflags;

	//Get the current eflags
	__asm{
		pushfd;				//Pushes eflags onto the stack
		mov edx, [esp];		//Take the memory at top of the stack
							//and put it into a register(edx is arbitrary)
							//This step is necessary because we can't
							//do a direct memory to memory move from the stack
							//directly to myEflags
		mov myEflags, edx;	//Puts the thing on top of the stack
							//into my C local variable
	};

	/*
	Regarding the ID flag, the manual says:
	"The ability of a program or procedure to set or clear 
	this flag indicates support for the CPUID instruction."
	*/

	//Recall that ID is bit [21]
	//binary: 00000000001000000000000000000000
	//hex: 0x00200000
	
	//If ID == 1
	if((myEflags & 0x00200000) == 0x00200000){
		//Attempt to clear the flag
		if(!ClearIDFlag()){
			printf("ID flag wasn't cleared. Can't use CPUID!\n");
			return 0;
		}
		return 1;
	}
	else{ //ID == 0]
		//Attempt to set the flag
		if(!SetIDFlag()){
			printf("ID flag wasn't set. Can't use CPUID!\n");
			return 0;
		}
		return 1;
	}
}

int main(){
	unsigned int maxBasicCPUID;
	char vendorString[13];
	char * vendorStringPtr = (char *)vendorString; //Move the address into its own register
												//because it makes the asm syntax easier

	//First we will check whether we can even use CPUID
	//Such a check is actually more complicated than it seems
	if(CheckIfWeCanUseCPUID() == 1){
		__asm{
			mov edi, vendorStringPtr; //Get the base address of the char[] into a register
			mov eax, 0; //We're going to do CPUID with input of 0
			cpuid;		//As stated, the instruction doesn't have any operands
			//Get back the results  which are now stored in eax, ebx, ecx, edx
			//and will have values as specified by the manual
			mov maxBasicCPUID, eax;
			mov [edi], ebx;//We order which register we put into which address
			mov [edi+4], edx;//so that they all end up forming a human readable string
			mov [edi+8], ecx;
		}
		vendorString[12] = 0;
		printf("maxBasicCPUID = %#x, vendorString = %s\n", maxBasicCPUID, vendorString);
	}
	else{
		printf("Utter failure\n");
	}
	return 0xb45eba11;
}