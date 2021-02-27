// ProtMode.cpp : Defines the entry point for the DLL application.
//
//Original file from: http://www.codeguru.com/cpp/w-p/system/devicedriverdevelopment/article.php/c8035/
//Modified by Xeno Kovah 12/30/2009 for MITRE Intermediate x86 class
//NOTE: this has a post-build event of
//copy debug\ProtMode.dll "C:\Program Files\Debugging Tools for Windows (x86)\winxp"
//so if you store your plugins somewhere else you will need to modify the event
//otherwise it will say it will not output final success when building

#include "stdafx.h"
#include "wdbgexts.h"
#include <stdio.h>

PWINDBG_EXTENSION_APIS g_pExtensionApis;
EXT_API_VERSION g_ApiVersion = { 3, 5, EXT_API_VERSION_NUMBER, 0 };

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// The initialization function.
VOID WDBGAPI WinDbgExtensionDllInit(
	 PWINDBG_EXTENSION_APIS lpExtensionApis,
	 USHORT usMajorVersion,
	 USHORT usMinorVersion)
{
	 // Save the pointers to the callback functions.
	 g_pExtensionApis = lpExtensionApis;
}
 
// WinDBG calls this to get the extension version.
LPEXT_API_VERSION WDBGAPI ExtensionApiVersion()
{
	return &g_ApiVersion;
}

//Xeno added

void DumpSegmentTypes(BOOL gdt)
{
	/////////////////////////////////////////
	// Get the descriptor data.
	/////////////////////////////////////////
	long lTableBase;
	short sTableSizeInBytes;
	short numEntries;

	if(gdt){
		// Get the GDT base address.
		lTableBase = g_pExtensionApis->lpGetExpressionRoutine("@GDTR");
		sTableSizeInBytes = g_pExtensionApis->lpGetExpressionRoutine("@GDTL");
		g_pExtensionApis->lpOutputRoutine("sTableSizeInBytes = %#x\n", sTableSizeInBytes);
		numEntries = (sTableSizeInBytes+1) / 8;
		g_pExtensionApis->lpOutputRoutine("------------------- Global Descriptor Table --------------------\r\n");
	}
	else{
		// Get the LDT base address.
		lTableBase = g_pExtensionApis->lpGetExpressionRoutine("@LDTR");
		if(lTableBase == 0){
			g_pExtensionApis->lpOutputRoutine("LDTR = 0. LDT Unused.\n");
			return;
		}
		sTableSizeInBytes = g_pExtensionApis->lpGetExpressionRoutine("@LDTL");
		g_pExtensionApis->lpOutputRoutine("sTableSizeInBytes = %#x\n", sTableSizeInBytes);
		numEntries = (sTableSizeInBytes+1) / 8;
		g_pExtensionApis->lpOutputRoutine("------------------- Local Descriptor Table --------------------\r\n");
	}

	for(long lTableIndex = 0; lTableIndex < numEntries; lTableIndex++){

		// Calculate the address of the descriptor we want to dump.
		// Each descriptor is 8 bytes.
		long lDescriptorAddress = lTableBase + 8 * lTableIndex;

		// Copy the descriptor into a buffer that we'll analyze.
		unsigned char Descriptor[8];
		ULONG ulBytesRead;
		g_pExtensionApis->lpReadProcessMemoryRoutine(
			lDescriptorAddress,
			Descriptor,
			8,
			&ulBytesRead);

		/////////////////////////////////////////
		// Interpret the descriptor.
		/////////////////////////////////////////
		BOOL bSystemSegment = (Descriptor[5] & (1<<4)) == 0;
		char chDPL = (Descriptor[5] >> 5) & 3;
		BOOL bDefaultSize32 = (Descriptor[6] & 0x40);


		//Added by Xeno
		if(gdt){
			g_pExtensionApis->lpOutputRoutine("GDT[0x%02x] = ", lTableIndex);
		}
		else{
			g_pExtensionApis->lpOutputRoutine("LDT[0x%02x] = ", lTableIndex);
		}
		unsigned char systemSegmentType = (Descriptor[5] & 0x0F);
		if(bSystemSegment == 0){
			if(bDefaultSize32){
				g_pExtensionApis->lpOutputRoutine("32 bit ");
			}
			else{
				g_pExtensionApis->lpOutputRoutine("16 bit ");
			}

			g_pExtensionApis->lpOutputRoutine("ring %d ", chDPL);

			switch(systemSegmentType){
				case 0:
					g_pExtensionApis->lpOutputRoutine("Data: Read-Only\n");
					break;
				case 1:
					g_pExtensionApis->lpOutputRoutine("Data: Read-Only, Accessed\n");
					break;
				case 2:
					g_pExtensionApis->lpOutputRoutine("Data: Read/Write\n");
					break;
				case 3:
					g_pExtensionApis->lpOutputRoutine("Data: Read/Write, Accessed\n");
					break;
				case 4:
					g_pExtensionApis->lpOutputRoutine("Data: Read-Only, Expand Down\n");
					break;
				case 5:
					g_pExtensionApis->lpOutputRoutine("Data: Read-Only, Expand Down, Accessed\n");
					break;
				case 6:
					g_pExtensionApis->lpOutputRoutine("Data: Read/Write, Expand Down\n");
					break;
				case 7:
					g_pExtensionApis->lpOutputRoutine("Data: Read/Write, Expand Down, Accessed\n");
					break;
				case 8:
					g_pExtensionApis->lpOutputRoutine("Code: Execute-Only\n");
					break;
				case 9:
					g_pExtensionApis->lpOutputRoutine("Code: Execute-Only, Accessed\n");
					break;
				case 10:
					g_pExtensionApis->lpOutputRoutine("Code: Execute/Read\n");
					break;
				case 11:
					g_pExtensionApis->lpOutputRoutine("Code: Execute/Read, Accessed\n");
					break;
				case 12:
					g_pExtensionApis->lpOutputRoutine("Code: Execute-Only, Conforming\n");
					break;
				case 13:
					g_pExtensionApis->lpOutputRoutine("Code: Execute-Only, Conforming, Accessed\n");
					break;
				case 14:
					g_pExtensionApis->lpOutputRoutine("Code: Execute/Read, Conforming\n");
					break;
				case 15:
					g_pExtensionApis->lpOutputRoutine("Code: Execute/Read, Conforming, Accessed\n");
					break;
			}
		}
		else{
			if(Descriptor[5] == 0){
				g_pExtensionApis->lpOutputRoutine("Empty Descriptor.\n");
				continue;
			}
			switch(systemSegmentType){
				case 0:
					g_pExtensionApis->lpOutputRoutine("Reserved 0. Shouldn't see this.\n");
					break;
				case 1:
					g_pExtensionApis->lpOutputRoutine("16-bit TSS (Available)\n");
					break;
				case 2:
					g_pExtensionApis->lpOutputRoutine("LDT\n");
					break;
				case 3:
					g_pExtensionApis->lpOutputRoutine("16-bit TSS (Busy)\n");
					break;
				case 4:
					g_pExtensionApis->lpOutputRoutine("16-bit Call Gate\n");
					break;
				case 5:
					g_pExtensionApis->lpOutputRoutine("Task Gate\n");
					break;
				case 6:
					g_pExtensionApis->lpOutputRoutine("16-bit Interrupt Gate\n");
					break;
				case 7:
					g_pExtensionApis->lpOutputRoutine("16-bit Trap Gate\n");
					break;
				case 8:
					g_pExtensionApis->lpOutputRoutine("Reserved 8. Shouldn't see this.\n");
					break;
				case 9:
					g_pExtensionApis->lpOutputRoutine("32-bit TSS (Available)\n");
					break;
				case 10:
					g_pExtensionApis->lpOutputRoutine("Reserved 10. Shouldn't see this.\n");
					break;
				case 11:
					g_pExtensionApis->lpOutputRoutine("32-bit TSS (Busy)\n");
					break;
				case 12:
					g_pExtensionApis->lpOutputRoutine("32-bit Call Gate)\n");
					break;
				case 13:
					g_pExtensionApis->lpOutputRoutine("Reserved 13. Shouldn't see this.\n");
					break;
				case 14:
					g_pExtensionApis->lpOutputRoutine("32-bit Interrupt Gate\n");
					break;
				case 15:
					g_pExtensionApis->lpOutputRoutine("32-bit Trap Gate\n");
					break;
			}
		}
	}//End for each entry
}

void DumpGDTOrLDTDescriptor(BOOL bLDT, long lTableIndex)
{
	/////////////////////////////////////////
	// Get the descriptor data.
	/////////////////////////////////////////

	// Get the GDT base address.
	long lTableBase = g_pExtensionApis->lpGetExpressionRoutine(bLDT ? "@LDTR" : "@GDTR");
	
	// Calculate the address of the descriptor we want to dump.
	// Each descriptor is 8 bytes.
	long lDescriptorAddress = lTableBase + 8 * lTableIndex;

	// Copy the descriptor into a buffer that we'll analyze.
	unsigned char Descriptor[8];
	ULONG ulBytesRead;
	g_pExtensionApis->lpReadProcessMemoryRoutine(
		lDescriptorAddress,
		Descriptor,
		8,
		&ulBytesRead);

	/////////////////////////////////////////
	// Print the raw data.
	/////////////////////////////////////////

	g_pExtensionApis->lpOutputRoutine("------------------- Code Segment Descriptor --------------------\r\n");

	// Print out the GDT base, index and resulting descriptor address.
	g_pExtensionApis->lpOutputRoutine("GDT base = 0x%08X, Index = 0x%02x, Descriptor @ 0x%08x\r\n", 
		lTableBase, lTableIndex, lDescriptorAddress);

	// Print out the raw descriptor data.
	g_pExtensionApis->lpOutputRoutine("%08x ", lDescriptorAddress);

	for(int i=0; i<8; i++)
	{
		g_pExtensionApis->lpOutputRoutine("%02x ", Descriptor[i]);
	}
	
	g_pExtensionApis->lpOutputRoutine("\r\n");

	/////////////////////////////////////////
	// Interpret the descriptor.
	/////////////////////////////////////////

	// Parse out the Granularity bit and the Default size bit.
	BOOL b4KBPages = (Descriptor[6] & (1<<7)) == (1<<7);
	BOOL bDefaultSize32 = (Descriptor[6] & (1<<6)) == (1<<6);

	// Print out the interpreted descriptor information.
	g_pExtensionApis->lpOutputRoutine("Segment size is in %s, %s-bit default operand and data size\r\n", 
		b4KBPages ? "4KB pages" : "bytes", bDefaultSize32 ? "32" : "16");
	
	// Parse out the Present, DPL, System bit and S bit.
	BOOL bSegmentPresent = (Descriptor[5] & (1<<7)) == (1<<7);
	char chDPL = (Descriptor[5] >> 5) & 3; 
	BOOL bSystemSegment = (Descriptor[5] & (1<<4)) == 0;
	BOOL bCodeSegment = (Descriptor[5] & (1<<3)) == (1<<3);

	// Print out the interpreted descriptor information.
	g_pExtensionApis->lpOutputRoutine("Segment %s present, DPL = %d, %s segment, %s segment\r\n", 
		bSegmentPresent ? "is" : "is not", 
		chDPL, 
		bSystemSegment ? "System" : "Not system", 
		bCodeSegment ? "Code" : "Data");

	//Xeno wrapped these prints in the if clause
	if(!bSystemSegment){
		// Parse out the Conforming, Readable and Accessed bits.
		BOOL bConformingSegment = (Descriptor[5] & (1<<2)) == (1<<2);
		BOOL bReadableSegment = (Descriptor[5] & (1<<1)) == (1<<1);
		BOOL bAccessedSegment = (Descriptor[5] & (1<<0)) == (1<<0);

		// Print out the interpreted descriptor information.
		g_pExtensionApis->lpOutputRoutine("Segment is %s, Segment is %s, Segment is %s\r\n", 
			bConformingSegment ? "conforming" : "not conforming", 
			bReadableSegment ? "readable" : "not readable", 
			bAccessedSegment ? "accessed" : "not accessed");
	}
	//Xeno added this entire else clause
	else{
		unsigned char systemSegmentType = (Descriptor[5] & 0x0F);
		g_pExtensionApis->lpOutputRoutine("System Segment is of Type: ");
		switch(systemSegmentType){
			case 0:
				if(Descriptor[5] == 0){
						g_pExtensionApis->lpOutputRoutine("Empty Descriptor.\n");
						break;
				}
				g_pExtensionApis->lpOutputRoutine("Reserved 0. Probably Empty.\n");
				break;
			case 1:
				g_pExtensionApis->lpOutputRoutine("16-bit TSS (Available)\n");
				break;
			case 2:
				g_pExtensionApis->lpOutputRoutine("LDT\n");
				break;
			case 3:
				g_pExtensionApis->lpOutputRoutine("16-bit TSS (Busy)\n");
				break;
			case 4:
				g_pExtensionApis->lpOutputRoutine("16-bit Call Gate\n");
				break;
			case 5:
				g_pExtensionApis->lpOutputRoutine("Task Gate\n");
				break;
			case 6:
				g_pExtensionApis->lpOutputRoutine("16-bit Interrupt Gate\n");
				break;
			case 7:
				g_pExtensionApis->lpOutputRoutine("16-bit Trap Gate\n");
				break;
			case 8:
				g_pExtensionApis->lpOutputRoutine("Reserved 8. Shouldn't see this.\n");
				break;
			case 9:
				g_pExtensionApis->lpOutputRoutine("32-bit TSS (Available)\n");
				break;
			case 10:
				g_pExtensionApis->lpOutputRoutine("Reserved 10. Shouldn't see this.\n");
				break;
			case 11:
				g_pExtensionApis->lpOutputRoutine("32-bit TSS (Busy)\n");
				break;
			case 12:
				g_pExtensionApis->lpOutputRoutine("32-bit Call Gate)\n");
				break;
			case 13:
				g_pExtensionApis->lpOutputRoutine("Reserved 13. Shouldn't see this.\n");
				break;
			case 14:
				g_pExtensionApis->lpOutputRoutine("32-bit Interrupt Gate\n");
				break;
			case 15:
				g_pExtensionApis->lpOutputRoutine("32-bit Trap Gate\n");
				break;
			default:
				break;
		}
	}

	// Parse out the base address of the code segment.
	unsigned long ulSegmentBaseAddress = (Descriptor[7] << 3*8) + 
		(Descriptor[4] << 2*8) + (Descriptor[3] << 1*8) + Descriptor[2];
	g_pExtensionApis->lpOutputRoutine("Target code segment base address = 0x%08x\r\n", 
		ulSegmentBaseAddress);
	
	// Parse out the segment size.
	unsigned long ulSegmentSize = ((Descriptor[6] & 0xf) << 16) + (Descriptor[1] << 8) + Descriptor[0];
	g_pExtensionApis->lpOutputRoutine("Target code segment size = 0x%08x\r\n", ulSegmentSize);

}

//Xeno added
void DumpIDTTypes()
{
	/////////////////////////////////////////
	// Get the descriptor data.
	/////////////////////////////////////////

	// Get the IDT base address.
	long lTableBase = g_pExtensionApis->lpGetExpressionRoutine("@IDTR");
	short sTableSizeInBytes = g_pExtensionApis->lpGetExpressionRoutine("@IDTL");
	g_pExtensionApis->lpOutputRoutine("sTableSizeInBytes = %#x\n", sTableSizeInBytes);
	short numEntries = (sTableSizeInBytes+1) / 8;

	g_pExtensionApis->lpOutputRoutine("------------------- Interrupt Descriptor Table --------------------\r\n");

	for(long lTableIndex = 0; lTableIndex < numEntries; lTableIndex++){

		// Calculate the address of the descriptor we want to dump.
		// Each descriptor is 8 bytes.
		long lDescriptorAddress = lTableBase + 8 * lTableIndex;

		// Copy the descriptor into a buffer that we'll analyze.
		unsigned char Descriptor[8];
		ULONG ulBytesRead;
		g_pExtensionApis->lpReadProcessMemoryRoutine(
			lDescriptorAddress,
			Descriptor,
			8,
			&ulBytesRead);

		/////////////////////////////////////////
		// Interpret the descriptor.
		/////////////////////////////////////////

		//Added by Xeno
		g_pExtensionApis->lpOutputRoutine("IDT[0x%02x] = ", lTableIndex);
		char gateType = (Descriptor[5] & 0xF);
		switch(gateType){
			case 0:
				g_pExtensionApis->lpOutputRoutine("NO ENTRY\n");
				break;
			case 5:
				g_pExtensionApis->lpOutputRoutine("TASK GATE\n");
				break;
			case 6:
				g_pExtensionApis->lpOutputRoutine("16 BIT INTERRUPT GATE\n");
				break;
			case 7:
				g_pExtensionApis->lpOutputRoutine("16 BIT TRAP GATE\n");
				break;
			case 0xE:
				g_pExtensionApis->lpOutputRoutine("32 BIT INTERRUPT GATE\n");
				break;
			case 0xF:
				g_pExtensionApis->lpOutputRoutine("32 BIT TRAP GATE\n");
				break;
		}
	}//End for each entry
}

//Xeno added dumpSegmentToo condition
void DumpIDTDescriptor(long lTableIndex, BOOL dumpSegmentToo)
{
	/////////////////////////////////////////
	// Get the descriptor data.
	/////////////////////////////////////////

	// Get the IDT base address.
	long lTableBase = g_pExtensionApis->lpGetExpressionRoutine("@IDTR");
	
	// Calculate the address of the descriptor we want to dump.
	// Each descriptor is 8 bytes.
	long lDescriptorAddress = lTableBase + 8 * lTableIndex;

	// Copy the descriptor into a buffer that we'll analyze.
	unsigned char Descriptor[8];
	ULONG ulBytesRead;
	g_pExtensionApis->lpReadProcessMemoryRoutine(
		lDescriptorAddress,
		Descriptor,
		8,
		&ulBytesRead);

	/////////////////////////////////////////
	// Print the raw data.
	/////////////////////////////////////////

	g_pExtensionApis->lpOutputRoutine("------------------- Interrupt Gate Descriptor --------------------\r\n");

	// Print out the IDT base, index and resulting descriptor address.
	g_pExtensionApis->lpOutputRoutine("IDT base = 0x%08X, Index = 0x%02x, Descriptor @ 0x%08x\r\n", 
		lTableBase, lTableIndex, lDescriptorAddress);

	// Print out the raw descriptor data.
	g_pExtensionApis->lpOutputRoutine("%08x ", lDescriptorAddress);

	for(int i=0; i<8; i++)
	{
		g_pExtensionApis->lpOutputRoutine("%02x ", Descriptor[i]);
	}
	
	g_pExtensionApis->lpOutputRoutine("\r\n");

	/////////////////////////////////////////
	// Interpret the descriptor.
	/////////////////////////////////////////

	// Parse out the Present, DPL, System bit and X bit.
	BOOL bSegmentPresent = (Descriptor[5] & (1<<7)) == (1<<7);
	char chDPL = (Descriptor[5] >> 5) & 3; 
	BOOL b32BitDescriptor = (Descriptor[5] & (1<<3)) == (1<<3);

	//Added by Xeno
	char gateType = (Descriptor[5] & 0xF);
	switch(gateType){
		case 0:
			g_pExtensionApis->lpOutputRoutine("NO ENTRY\n");
			break;
		case 5:
			g_pExtensionApis->lpOutputRoutine("TASK GATE\n");
			break;
		case 6:
			g_pExtensionApis->lpOutputRoutine("16 BIT INTERRUPT GATE\n");
			break;
		case 7:
			g_pExtensionApis->lpOutputRoutine("16 BIT TRAP GATE\n");
			break;
		case 0xE:
			g_pExtensionApis->lpOutputRoutine("32 BIT INTERRUPT GATE\n");
			break;
		case 0xF:
			g_pExtensionApis->lpOutputRoutine("32 BIT TRAP GATE\n");
			break;
	}

	//Xeno removed incorrect reference to segment type detection which cannot be done
	//from an interrupt descriptor
	// Print out the interpreted descriptor information.
	g_pExtensionApis->lpOutputRoutine("Segment %s present, DPL = %d, %s-bit descriptor\r\n", 
		bSegmentPresent ? "is" : "is not", chDPL, b32BitDescriptor ? "32" : "16");

	// Parse out the target code segment selector.
	WORD wCodeSegmentSelector = (Descriptor[3] << 1) + Descriptor[2];
	WORD wDTIndex = wCodeSegmentSelector >> 3;
	BOOL bLDT = (wCodeSegmentSelector & (1<<2)) == (1<<2);
	char chRPL = wCodeSegmentSelector & 3; 
	
	// Print out the interpreted target code selector information.
	//Xeno fixed index to be in hex since the input for indices is in hex anyway and this could be confusing
	g_pExtensionApis->lpOutputRoutine("Target code segment selector = 0x%04x (%s Index = %#x, RPL = %d)\r\n", 
		wCodeSegmentSelector, bLDT ? "LDT" : "GDT", wDTIndex, chRPL);

	//Xeno: It doesn't make sense to print out a segment offset for a task gate
	if(gateType != 5){
		// Parse out the target segment offset.
		unsigned long ulTargetSegmentOffset = (Descriptor[7] << 3*8) + 
			(Descriptor[6] << 2*8) + (Descriptor[1] << 1*8) + Descriptor[0];
	
		g_pExtensionApis->lpOutputRoutine("Target code segment offset = 0x%08x\r\n", 
			ulTargetSegmentOffset);
	}

	//Xeno made this conditional
	if(dumpSegmentToo){
		// Dump out the Code Segment Descriptor.
		DumpGDTOrLDTDescriptor(bLDT, wDTIndex);
	}
}

//Xeno added
void PrintTSS32(long lMemLoc)
{
	/////////////////////////////////////////
	// Get the descriptor data.
	/////////////////////////////////////////

	// Copy the descriptor into a buffer that we'll analyze.
	unsigned long TSS[26];
	ULONG ulBytesRead;
	g_pExtensionApis->lpReadProcessMemoryRoutine(
		lMemLoc,
		TSS,
		26*sizeof(long),
		&ulBytesRead);

	if(ulBytesRead != 26 * sizeof(long)){
		g_pExtensionApis->lpOutputRoutine("Error reading the required memory amount\n");
		return;
	}

	/////////////////////////////////////////
	// Interpret the TSS.
	/////////////////////////////////////////

	g_pExtensionApis->lpOutputRoutine("Previous task link = %#x\n", TSS[0] & 0xFFFF);
	g_pExtensionApis->lpOutputRoutine("ESP0 = \t%#x\n", TSS[1]);
	g_pExtensionApis->lpOutputRoutine("SS0 = \t%#x\n", TSS[2] & 0xFFFF);
	g_pExtensionApis->lpOutputRoutine("ESP1 = \t%#x\n", TSS[3]);
	g_pExtensionApis->lpOutputRoutine("SS1 = \t%#x\n", TSS[4] & 0xFFFF);
	g_pExtensionApis->lpOutputRoutine("ESP2 = \t%#x\n", TSS[5]);
	g_pExtensionApis->lpOutputRoutine("SS2 = \t%#x\n", TSS[6] & 0xFFFF);
	g_pExtensionApis->lpOutputRoutine("CR3 = \t%#x\n", TSS[7]);
	g_pExtensionApis->lpOutputRoutine("EIP = \t%#x\n", TSS[8]);
	g_pExtensionApis->lpOutputRoutine("EFLAGS = \t%#x\n", TSS[9]);
	g_pExtensionApis->lpOutputRoutine("EAX = \t%#x\n", TSS[10]);
	g_pExtensionApis->lpOutputRoutine("ECX = \t%#x\n", TSS[11]);
	g_pExtensionApis->lpOutputRoutine("EDX = \t%#x\n", TSS[12]);
	g_pExtensionApis->lpOutputRoutine("EBX = \t%#x\n", TSS[13]);
	g_pExtensionApis->lpOutputRoutine("ESP = \t%#x\n", TSS[14]);
	g_pExtensionApis->lpOutputRoutine("EBP = \t%#x\n", TSS[15]);
	g_pExtensionApis->lpOutputRoutine("ESI = \t%#x\n", TSS[16]);
	g_pExtensionApis->lpOutputRoutine("EDI = \t%#x\n", TSS[17]);
	g_pExtensionApis->lpOutputRoutine("ES = \t%#x\n", TSS[18] & 0xFFFF);
	g_pExtensionApis->lpOutputRoutine("CS = \t%#x\n", TSS[19] & 0xFFFF);
	g_pExtensionApis->lpOutputRoutine("SS = \t%#x\n", TSS[20] & 0xFFFF);
	g_pExtensionApis->lpOutputRoutine("DS = \t%#x\n", TSS[21] & 0xFFFF);
	g_pExtensionApis->lpOutputRoutine("FS = \t%#x\n", TSS[22] & 0xFFFF);
	g_pExtensionApis->lpOutputRoutine("GS = \t%#x\n", TSS[23] & 0xFFFF);
	g_pExtensionApis->lpOutputRoutine("LDT segment selector = %#x\n", TSS[24] & 0xFFFF);
	g_pExtensionApis->lpOutputRoutine("I/O map base = \t%#x\n\n", TSS[25] >> 16);

}

//Xeno added
void PrintTSS16(long lMemLoc)
{
	/////////////////////////////////////////
	// Get the descriptor data.
	/////////////////////////////////////////

	// Copy the descriptor into a buffer that we'll analyze.
	unsigned short TSS[22];
	ULONG ulBytesRead;
	g_pExtensionApis->lpReadProcessMemoryRoutine(
		lMemLoc,
		TSS,
		26*sizeof(short),
		&ulBytesRead);

	if(ulBytesRead != 26 * sizeof(short)){
		g_pExtensionApis->lpOutputRoutine("Error reading the required memory amount\n");
		return;
	}

	/////////////////////////////////////////
	// Interpret the TSS.
	/////////////////////////////////////////

	g_pExtensionApis->lpOutputRoutine("Previous task link = %#x\n", TSS[0]);
	g_pExtensionApis->lpOutputRoutine("SP0 = \t%#x\n", TSS[1]);
	g_pExtensionApis->lpOutputRoutine("SS0 = \t%#x\n", TSS[2]);
	g_pExtensionApis->lpOutputRoutine("SP1 = \t%#x\n", TSS[3]);
	g_pExtensionApis->lpOutputRoutine("SS1 = \t%#x\n", TSS[4]);
	g_pExtensionApis->lpOutputRoutine("SP2 = \t%#x\n", TSS[5]);
	g_pExtensionApis->lpOutputRoutine("SS2 = \t%#x\n", TSS[6]);
	g_pExtensionApis->lpOutputRoutine("IP = \t%#x\n", TSS[7]);
	g_pExtensionApis->lpOutputRoutine("FLAGS = \t%#x\n", TSS[8]);
	g_pExtensionApis->lpOutputRoutine("AX = \t%#x\n", TSS[9]);
	g_pExtensionApis->lpOutputRoutine("CX = \t%#x\n", TSS[10]);
	g_pExtensionApis->lpOutputRoutine("DX = \t%#x\n", TSS[11]);
	g_pExtensionApis->lpOutputRoutine("BX = \t%#x\n", TSS[12]);
	g_pExtensionApis->lpOutputRoutine("SP = \t%#x\n", TSS[13]);
	g_pExtensionApis->lpOutputRoutine("BP = \t%#x\n", TSS[14]);
	g_pExtensionApis->lpOutputRoutine("SI = \t%#x\n", TSS[15]);
	g_pExtensionApis->lpOutputRoutine("DI = \t%#x\n", TSS[16]);
	g_pExtensionApis->lpOutputRoutine("ES = \t%#x\n", TSS[17]);
	g_pExtensionApis->lpOutputRoutine("CS = \t%#x\n", TSS[18]);
	g_pExtensionApis->lpOutputRoutine("SS = \t%#x\n", TSS[19]);
	g_pExtensionApis->lpOutputRoutine("DS = \t%#x\n", TSS[20]);
	g_pExtensionApis->lpOutputRoutine("LDT segment selector = %#x\n", TSS[21]);
}

DECLARE_API(descriptor)
{
	// These are the arguments passed in:
	HANDLE _hCurrentProcess = hCurrentProcess;
    HANDLE _hCurrentThread = hCurrentThread;
    ULONG _dwCurrentPc = dwCurrentPc;
    ULONG _dwProcessor = dwProcessor;
    PCSTR _args = args;

	// Parse out the arguments.
	// The command arguments are (in args):
	// 1: "IDT", "GDT" or "LDT". 
	//Xeno added IDT_FULL which behaves like the original IDT option
	//Xeno added DUMP_IDT_TYPES which prints the type for each IDT entry
	//Xeno added DUMP_GDT_TYPES which prints the type for each GDT entry
	//Xeno added DUMP_LDT_TYPES which prints the type for each LDT entry
	//Xeno added TSS32 which prints memory as if it were a 32 bit TSS
	//Xeno added TSS16 which prints memory as if it were a 16 bit TSS
	// 2: The index into the descriptor table.

	char szTable[128];
	long lIndex;
	sscanf(args, "%s %x", szTable, &lIndex);

	// Dump the specified descriptor table entry.
	if(_stricmp(szTable, "IDT") == 0)
	{
		//Xeno made this form the default
		//This form does not dump the GDT/LDT into
		DumpIDTDescriptor(lIndex, false);
		return;
	}

	//Xeno added this form
	//This mimics the original behavior of dumping the GDT/LDT entry as well
	if(_stricmp(szTable, "IDT_FULL") == 0)
	{
		DumpIDTDescriptor(lIndex, true);
		return;
	}

	//Xeno added: Dump the entire IDT.
	if(_stricmp(szTable, "DUMP_IDT_TYPES") == 0)
	{
		DumpIDTTypes();
		return;
	}


	if(_stricmp(szTable, "GDT") == 0)
	{
		DumpGDTOrLDTDescriptor(FALSE, lIndex);
		return;
	}

	//Xeno added: Dump the entire GDT.
	if(_stricmp(szTable, "DUMP_GDT_TYPES") == 0)
	{
		DumpSegmentTypes(TRUE);
		return;
	}


	if(_stricmp(szTable, "LDT") == 0)
	{
		DumpGDTOrLDTDescriptor(TRUE, lIndex);
		return;
	}

	//Xeno added: Dump the entire LDT.
	if(_stricmp(szTable, "DUMP_LDT_TYPES") == 0)
	{
		DumpSegmentTypes(FALSE);
		return;
	}

	//Xeno added: Print a 32 bit TSS
	if(_stricmp(szTable, "TSS32") == 0)
	{
		PrintTSS32(lIndex);
		return;
	}

	//Xeno added: Print a 16 bit TSS
	if(_stricmp(szTable, "TSS16") == 0)
	{
		PrintTSS16(lIndex);
		return;
	}

	g_pExtensionApis->lpOutputRoutine("Acceptable commands are:\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor IDT <descriptor index in hex>\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps only information about the IDT entry.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor IDT_FULL <descriptor index in hex>\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps information about the IDT entry and the segment it points at.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor DUMP_IDT_TYPES\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps a summary of the gate type for each IDT entry.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor GDT <descriptor index in hex>\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps information about the GDT entry.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor DUMP_GDT_TYPES\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps a summary of the segment type for each GDT entry.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor LDT <descriptor index in hex>\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps information about the LDT entry.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor DUMP_LDT_TYPES\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps a summary of the segment type for each LDT entry.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor TSS32 <base memory address>\n");
	g_pExtensionApis->lpOutputRoutine("\t Prints what a 32 bit TSS based as <base memory address> would look like.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor TSS16 <base memory address>\n");
	g_pExtensionApis->lpOutputRoutine("\t Prints what a 16 bit TSS based as <base memory address> would look like.\n\n");
}

DECLARE_API(help)
{
	g_pExtensionApis->lpOutputRoutine("Acceptable commands are:\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor IDT <descriptor index in hex>\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps only information about the IDT entry.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor IDT_FULL <descriptor index in hex>\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps information about the IDT entry and the segment it points at.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor DUMP_IDT_TYPES\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps a summary of the gate type for each IDT entry.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor GDT <descriptor index in hex>\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps information about the GDT entry.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor DUMP_GDT_TYPES\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps a summary of the segment type for each GDT entry.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor LDT <descriptor index in hex>\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps information about the LDT entry.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor DUMP_LDT_TYPES\n");
	g_pExtensionApis->lpOutputRoutine("\t Dumps a summary of the segment type for each LDT entry.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor TSS32 <base memory address>\n");
	g_pExtensionApis->lpOutputRoutine("\t Prints what a 32 bit TSS based as <base memory address> would look like.\n\n");
	g_pExtensionApis->lpOutputRoutine("!descriptor TSS16 <base memory address>\n");
	g_pExtensionApis->lpOutputRoutine("\t Prints what a 16 bit TSS based as <base memory address> would look like.\n\n");
}
