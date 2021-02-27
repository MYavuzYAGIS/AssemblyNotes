//BreakOnThruToTheOtherSide.c: Compliment to TryToRunTryToHide.c which prints the same info
//Written by Xeno Kovah, Jan. 16th 2010
//Originally for use with MITRE Institute Intermediate x86 Class
//Licensed under Creative Commons Share Alike license.
//full details available here: http://creativecommons.org/licenses/by-sa/3.0
//P.s. Don't comment on the efficency or style of the code, it's tutorial code! :P

#include <ntddk.h>

//You can change this to hook other Interrupt/Trap gate entries
//in the IDT (NOTE!: Will not work with task gates! You've been warned!)
#define HOOKED_HANDLER_INDEX 0xEE

//You don't have to understand this yet
typedef struct interrupt_descriptor{
	unsigned short offsetLow;
	unsigned short segSelect;
	unsigned short dontcare:13;	//bits we don't care about for this example
	unsigned short DPL:2;		//The two descriptor privilege level bits
	unsigned short dontcare2:1;	//bits we don't care about for this example
	unsigned short offsetHigh;
} int_desc_t;

//global to store a pointer to the descriptor entry in the IDT
//so that I don't have to find it twice (once on load and unload)
int_desc_t * gDesciptorPtr;

//global to store the original address pointed to by the handler
unsigned int gOrigHandlerAddress;
//global to store the original DPL of the handler
unsigned char gOrigDPL;

//prototypes for helper functions
void MyInterruptHandler();
//BEGIN boilerplate
NTSTATUS DefaultDispatch(PDEVICE_OBJECT DriverObject, PIRP pIRP);
void OnUnload(PDRIVER_OBJECT DriverObject);
//END boilerplate

//Function called when driver is loaded - equivalent of main()
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) 
{
	unsigned int i;
	unsigned char myIDTR[6];
	unsigned char * myIDTRPtr;
	unsigned char * myIDTRPtr2;
	unsigned int idtBase;
	unsigned int myHandlerPtr;

	//Initialize the globals
	gOrigHandlerAddress = 0;
	gOrigDPL = 0;

	//The low 2 bytes are the size limit and we want to skip that
	//and go right to the base address later when we read the
	//stored IDTR information
	myIDTRPtr = (unsigned char *)&(myIDTR[2]);

	//BEGIN boilerplate
	for(i = 0;i < IRP_MJ_MAXIMUM_FUNCTION;i++){
		DriverObject->MajorFunction[i] = DefaultDispatch;
	}
	DriverObject->DriverUnload = OnUnload;
	//END boilerplate

	//I'm going to replace the existing interrupt handler with my own
	//so that when userspace invokes "int HOOKED_HANDLER_INDEX" it will go to me first, and
	//then I will let it go to the original handler
	//You don't need to understand this part for now, but you can come back
	//and look at it after we talk about descriptors.
	//Note, I am basically "hooking" the interrupt in the same way that a
	//rootkit might redirect control to itself

	//Dump the IDTR to 6 bytes of memory pointed to by myIDTRPtr
	__asm{
		sidt myIDTR;
	}

	//Get the base (the upper 4 bytes)
	idtBase = *(unsigned int *)myIDTRPtr;

	//Get the address of the HOOKED_HANDLER_INDEX descriptor
	gDesciptorPtr = (int_desc_t *)(idtBase + HOOKED_HANDLER_INDEX * sizeof(int_desc_t));
	
	//Store the existing pointer and DPL so we can put them back when we're done
	gOrigHandlerAddress = (gDesciptorPtr->offsetHigh << 16) | gDesciptorPtr->offsetLow;
	gOrigDPL = (gDesciptorPtr->DPL & 0x3);

	//Store my function pointer in an int for convenience
	myHandlerPtr = (unsigned int)MyInterruptHandler;
	
	//Rewrite the destination handler address in the descriptor
	gDesciptorPtr->offsetHigh = (myHandlerPtr >> 16) & 0xFFFF;
	gDesciptorPtr->offsetLow = myHandlerPtr & 0xFFFF;

	//Set its privilege level to 3 so that userspace code can call it
	gDesciptorPtr->DPL = 3;

	return STATUS_SUCCESS; 
}

//Declare our interrupt handler
void MyInterruptHandler(){
	unsigned short myCS, myDS, myES, myFS, myGS, mySS, myTR;
	unsigned int myEAX, myEBX, myECX, myEDX, myEDI, myESI, myEFLAGS, myESP, myEBP;
	__asm{
		pushad;//push all the general purpose registers
	};
	DbgPrint("Break on through, to the other side!!!\n");
	
	//DbgPrint smashed some registers, so restore from the copies
	__asm{
		popad;//pop all the general purpose registers
	};
	

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
		pop eax; //restore eax because we're done clobbering it
	}

	DbgPrint("tr = %#x\n", myTR);
	DbgPrint("cs = %#x\n", myCS);
	DbgPrint("ss = %#x\n", mySS);
	DbgPrint("ds = %#x\n", myDS);
	DbgPrint("es = %#x\n", myES);
	DbgPrint("fs = %#x\n", myFS);
	DbgPrint("gs = %#x\n", myGS);
	DbgPrint("eax = %#x\n", myEAX);
	DbgPrint("ebx = %#x\n", myEBX);
	DbgPrint("ecx = %#x\n", myECX);
	DbgPrint("edx = %#x\n", myEDX);
	DbgPrint("edi = %#x\n", myEDI);
	DbgPrint("esi = %#x\n", myESI);
	DbgPrint("ebp = %#x\n", myEBP);
	DbgPrint("esp = %#x\n", myESP);
	DbgPrint("myEFLAGS = %#x\n", myEFLAGS);

	//Parse the eflags per Vol 3a, Sect 2.3
//	DbgPrint("EFLAGS: Trap Flag = %d\n", (myEFLAGS >> 8) & 0x1);
	DbgPrint("EFLAGS: Interrupt Flag = %d\n", (myEFLAGS >> 9) & 0x1);
//	DbgPrint("EFLAGS: IOPL (ignore for now) = %d\n", (myEFLAGS >> 12) & 0x3);
//	DbgPrint("EFLAGS: Resume Flag = %d\n", (myEFLAGS >> 16) & 0x1);


	//Now we have the option to either return from the interrupt or call the original handler

	//return back to userspace directly
	__asm {
		add esp, 0x48; //Compensate for local variable & caller save register space 
						//(including ebp). I calculated 0x48 by just looking at the
						//assembly in the function prolog before the pusha
		iretd; //gotta use the d or visual studio overrides it to a 16 bit iret. bah!
	};

	//redirect control back to the original interrupt handler
//	__asm{
//		add esp, 0x48; //Compensate for local variable & caller save register space 
//						//(including ebp). I calculated 0x48 by just looking at the
//						//assembly in the function prolog before the pusha
//		popad; //Pop off all the values we pushed with the pushad
//		jmp gOrigHandlerAddress;
//	};

}


//Function called when driver is unloaded
void OnUnload(PDRIVER_OBJECT DriverObject){
	
	//Restore the original values to the interrupt descriptor

	//Rewrite the destination handler address in the descriptor
	gDesciptorPtr->offsetHigh = (gOrigHandlerAddress >> 16) & 0xFFFF;
	gDesciptorPtr->offsetLow = gOrigHandlerAddress & 0xFFFF;

	//Set its privilege level to 3 so that userspace code can call it
	gDesciptorPtr->DPL = gOrigDPL;

	DbgPrint("Goodbye Kernel! I left things the way I found them :)\n");
}

//Boilerplate, ignore
NTSTATUS DefaultDispatch(PDEVICE_OBJECT DriverObject, PIRP pIRP){
	pIRP->IoStatus.Status = STATUS_SUCCESS;
	pIRP->IoStatus.Information = 0;
	IoCompleteRequest(pIRP, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}