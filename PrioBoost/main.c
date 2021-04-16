#include <ntifs.h>
#include <ntddk.h>
#include "Common.h"


#define PRIORITY_DEVICE 0x8000

// Setting up the control code
#define IOCTL_SET_PRIORITY CTL_CODE(PRIORITY_DEVICE , 0x800 ,\
METHOD_NEITHER , FILE_ANY_ACCESS)


// Declarations
_Use_decl_annotations_
NTSTATUS PrioBoostCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {

	// DeviceObject not needed here 
	UNREFERENCED_PARAMETER(DeviceObject); 

	Irp->IoStatus.Status = STATUS_SUCCESS; 
	Irp->IoStatus.Information = 0; 
	IoCompleteRequest(Irp, IO_NO_INCREMENT); 
	
	return STATUS_SUCCESS; 

}


_Use_decl_annotations_
NTSTATUS PriorityBoosterDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {

	// DeviceObject not needed here 
	UNREFERENCED_PARAMETER(DeviceObject);

	//get the IO_STACK_LOCATION 
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp); 

	NTSTATUS status = STATUS_SUCCESS; 

	switch (stack->Parameters.DeviceIoControl.IoControlCode) {
		
		case IOCTL_SET_PRIORITY: // Main Functionality here 

			if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(ThreadData) ) { // Checking if the buffer is large to contain our Thread structure
				status = STATUS_BUFFER_TOO_SMALL; 
				break; 
			}


			ThreadData* data= (ThreadData*)stack->Parameters.DeviceIoControl.Type3InputBuffer; // Extracting our buffer 
			
			if (!data) {
				status = STATUS_INVALID_PARAMETER; 
				break; 
			}

			if (data->Priority < 1 || data->Priority > 31) { // Possible values for priority are 1 ... 31 
				status = STATUS_INVALID_PARAMETER;
				break;
			}
			// Now lets get the pointer to the real thread object in kernel space 
			PETHREAD Thread; 
			status = PsLookupThreadByThreadId(UlongToHandle(data->ThreadId) , &Thread); 
			if (!NT_SUCCESS(status)) {
				break; // Leave the status like it was from the last return . 
			}


			// Now lets change The priority 
			KeSetPriorityThread((PKTHREAD)Thread, data->Priority); 
			
			KdPrint(("Priority change for Thread : %d to %d\n",data->ThreadId , data->Priority)); 

			// We Derefenrence the object After Success
			ObDereferenceObject(Thread);

			break; 

		default : 
			status = STATUS_INVALID_DEVICE_REQUEST; 
			break; 
	}
	
	
	// Resending informations to client 
	Irp->IoStatus.Status = status; 
	Irp->IoStatus.Information = 0; 
	IoCompleteRequest(Irp, IO_NO_INCREMENT); 
	
	return status;

}


void unload(_In_ PDRIVER_OBJECT DriverObject) {

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\PrioBoost");

	// Delete Symbolic link
	IoDeleteSymbolicLink(&symLink);

	// Delete device object 
	IoDeleteDevice(DriverObject->DeviceObject);

}


NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {

	UNREFERENCED_PARAMETER(RegistryPath); 
	
	// Setting up the Unload routine
	DriverObject->DriverUnload = unload ;

	// Setting up the Dispatch routines , Create and Close does the same thing , they are just checking the request 
	DriverObject->MajorFunction[IRP_MJ_CREATE] = PrioBoostCreateClose; 
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = PrioBoostCreateClose; 
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = PriorityBoosterDeviceControl; 

	//Creating the DeviceObject 
	UNICODE_STRING Device_name = RTL_CONSTANT_STRING(L"\\Device\\PrioBoost"); 

	PDEVICE_OBJECT DeviceObject; 

	NTSTATUS status = IoCreateDevice(
		DriverObject, 
		0, // no extra bytes
		&Device_name, // device name 
		FILE_DEVICE_UNKNOWN, // type of device
		0,
		FALSE,
		&DeviceObject // The pointer to our created Device
);

	if (!NT_SUCCESS(status)) {

		KdPrint(("Failed to create device object : 0x%08x\n", status)); 
		return status;
	}

	// Creating the symbolic link so we can connect to from user mode 
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\PrioBoost"); 

	status = IoCreateSymbolicLink(&symLink, &Device_name); 

	if (!NT_SUCCESS(status)) {

		KdPrint(("Failed to create Symlink : 0x%08x\n", status));
		IoDeleteDevice(DeviceObject); // Deleting the Device before leaving 
		return status;

	}
		
		return STATUS_SUCCESS; 
}




