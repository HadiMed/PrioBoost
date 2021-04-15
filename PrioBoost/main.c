#include <ntddk.h>

NTSTATUS PrioBoostCreateClose(_In_ PDEVICE_OBJECT DeviceObject , _In_ PIRP Irp); 


NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
	
	// Setting up the Unload routine
	DriverObject->DriverUnload = unload ;

	// Setting up the Dispatch routines
	DriverObject->MajorFunction[IRP_MJ_CREATE] = PrioBoostCreateClose; 
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = PrioBoostCreateClose; 




		
		
		
		
		
		
		
		
		return STATUS_SUCCESS; 
}

void unload(_In_ PDRIVER_OBJECT DriverObject) {

}
