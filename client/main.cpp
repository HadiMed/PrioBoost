#include <iostream>
#include <stdio.h>
#include <Windows.h>

typedef struct ThreadData {
	// ThreadId is 32 bit unsigned long, 
	//Priority is an int between 1 and 31 . 

	ULONG ThreadId;
	int Priority;
}ThreadData;

#define PRIORITY_DEVICE 0x8000

// Setting up the control code
#define IOCTL_SET_PRIORITY CTL_CODE(PRIORITY_DEVICE , 0x800 ,\
METHOD_NEITHER , FILE_ANY_ACCESS)


int main(int argc, char** argv) {
	
	if (argc < 3) {
		printf("Usage Priboost <TID> <priority>\n");
		exit(1); 
	}

	ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData)); 
	
	 
	data->ThreadId = atoi(argv[1]); 
	data->Priority = atoi(argv[2]);

;

	
	HANDLE Device = CreateFile(L"\\\\.\\PrioBoost", GENERIC_WRITE,
		FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr); 


	if (Device == INVALID_HANDLE_VALUE) {
		printf("Invalid handle\n"); 
		exit(1); 
	}

	DWORD returned;
	
	BOOL result = DeviceIoControl(
		Device,
		IOCTL_SET_PRIORITY,
		data, sizeof(data),
		nullptr, 0,
		&returned, nullptr);

	if (result)
		printf("Priority changed Successfully\n");
	else
		printf("Error setting up your prio"); 



	}
 
