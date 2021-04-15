#pragma once

struct ThreadData {
	// ThreadId is 32 bit unsigned long, 
	//Priority is an int between 1 and 31 . 

	ULONG ThreadId;
	int Priority;
};