//KernelspaceSegmentRegisters.c: Code to interpret kernel segment selectors
//Written by Xeno Kovah, Jan. 9th 2010
//Originally for use with MITRE Institute Intermediate x86 Class
//Licensed under Creative Commons Share Alike license.
//full details available here: http://creativecommons.org/licenses/by-sa/3.0
//P.s. Don't comment on the efficency or style of the code, it's tutorial code! :P

#include <ntddk.h> 

//prototypes for helper functions
void SelectorPrint(char * segRegName, unsigned short segReg);
//BEGIN boilerplate
NTSTATUS DefaultDispatch(PDEVICE_OBJECT DriverObject, PIRP pIRP);
void OnUnload(PDRIVER_OBJECT DriverObject);
//END boilerplate

//Function called when driver is loaded - equivalent of main()
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) 
{
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

	return 0xd011face;

}

//Function called when driver is unloaded
void OnUnload(PDRIVER_OBJECT DriverObject){
	DbgPrint("Goodbye Kernel!\n");
}

//Helper function to interpret segment selectors
void SelectorPrint(char * segRegName, unsigned short segReg){
	DbgPrint("The segment selector stored in the %s register = %#x\n", segRegName, segReg);
	if(segReg == 0){
		DbgPrint("A segment selector of 0 is invalid because the 0th entry of the GDT is never used\n\n");
		return;
	}
	DbgPrint("\tIts Requested Privilege Level is %u\n", (segReg & 0x3));
	DbgPrint("\tIt selects a segment in the ");
	if((segReg & 0x4) == 0){
		DbgPrint("GDT ");
	}
	else{
		DbgPrint("LDT");
	}
	DbgPrint(" of index %#x\n\n", segReg >> 3);
}

//Boilerplate, ignore
NTSTATUS DefaultDispatch(PDEVICE_OBJECT DriverObject, PIRP pIRP){
	pIRP->IoStatus.Status = STATUS_SUCCESS;
	pIRP->IoStatus.Information = 0;
	IoCompleteRequest(pIRP, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

