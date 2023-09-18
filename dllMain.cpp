#include <Windows.h>

#include "GTA.h"

// Function called by Windows, including on DLL "entry point" once it loads within our target process
BOOL WINAPI DllMain(__in  HINSTANCE hinstDLL, __in  DWORD fdwReason, __in  LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// Call the init function once the DLL is attached to a process
		init();
	}
	return TRUE;
}

WantedLevel wantedLevel;
Pedestrian player;

void init() {
	wantedLevel = hookWantedLevel(*pWantedLevelBaseAddress);
	player = hookPedestrian(*pPlayerPedBaseAddress);
}

void hack() {

}