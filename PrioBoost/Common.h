#pragma once

typedef struct ThreadData {
	// ThreadId is 32 bit unsigned long, 
	//Priority is an int between 1 and 31 . 

	ULONG ThreadId;
	int Priority;
}ThreadData;

//Prototypes
NTSTATUS PrioBoostCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

NTSTATUS PriorityBoosterDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);