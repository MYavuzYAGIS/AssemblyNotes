//VerboseRedPill.c: A version of Joanna Rutkowska's RedPill which is easier to read
//Original version http://www.invisiblethings.org/papers/redpill.html
//Instructions Learned: sidt
//Written by Xeno Kovah, Jan. 23rd 2010
//Originally for use with MITRE Institute Intermediate x86 Class
//Licensed under Creative Commons Share Alike license.
//full details available here: http://creativecommons.org/licenses/by-sa/3.0
//P.s. Don't comment on the efficency or style of the code, it's tutorial code! :P

#include <stdio.h>

int main(){
	unsigned char storage[6];//6 bytes of storage to store the IDTR into
	memset(storage, 0, 6);//Just to make sure it's clean

	//Remember that the lower 2 bytes of the IDTR are the size of the IDT in bytes
	//The upper 4 bytes are the 4 byte virtual address of the base of the IDT
	//But remember that everything will be stored in little-endian order
	__asm{
		sidt storage; //Store IDTR instruction
	}

	//Now storage[0],storage[1] hold the size which we don't care about
	//and storage[2] through storage[5] hold the

	//The key to RedPill is that it was found that the most significant byte
	//of the IDTR (and hence the virtual address pointing to the IDT) is
	//different in a virtualized system vs. an normal system.

	//The original code assumed that in a virtualized system the MSB is
	//0xD0, but I changed it to fit with the current observed value
	if(storage[5] == 0xFF){
		printf("This code being run in VMWare\n");
	}
	//One can imagine putting other values known for other
	//virtualization systems or versions of windows here
	else{
		printf("This code being run on a normal system\n");
	}

	return 0xca55e77e;
}