//HelloKernel.c: Skeleton kernel driver to base other code on
//Written by Xeno Kovah, Jan. 3rd 2010
//Originally for use with MITRE Institute Intermediate x86 Class
//Licensed under Creative Commons Share Alike license.
//full details available here: http://creativecommons.org/licenses/by-sa/3.0
//P.s. Don't comment on the efficency or style of the code, it's tutorial code! :P

#include <ntddk.h>

//prototypes for helper functions
//BEGIN boilerplate
NTSTATUS DefaultDispatch(PDEVICE_OBJECT DriverObject, PIRP pIRP);
void OnUnload(PDRIVER_OBJECT DriverObject);
//END boilerplate

//Function called when driver is loaded - equivalent of main()
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) 
{
	unsigned int i;
    DbgPrint("Hello Kernel!\n");
 
	//BEGIN boilerplate
	for(i = 0;i < IRP_MJ_MAXIMUM_FUNCTION;i++){
		DriverObject->MajorFunction[i] = DefaultDispatch;
	}
	DriverObject->DriverUnload = OnUnload;
	//END boilerplate
	return STATUS_SUCCESS; 
}

//Function called when driver is unloaded
void OnUnload(PDRIVER_OBJECT DriverObject){
	DbgPrint("Goodbye Kernel!\n");
}

//Boilerplate, ignore
NTSTATUS DefaultDispatch(PDEVICE_OBJECT DriverObject, PIRP pIRP){
	pIRP->IoStatus.Status = STATUS_SUCCESS;
	pIRP->IoStatus.Information = 0;
	IoCompleteRequest(pIRP, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}