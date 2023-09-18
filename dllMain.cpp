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
Vehicle vehicle;

Cheat* cheats = {
	new Cheat(nullptr, nullptr, &noWantedLevel),
	new Cheat(nullptr, &infiniteHealthOff, &infiniteHealth),
	new Cheat(nullptr, nullptr, &infiniteCarHealth),
	nullptr
};

void init() {
	hookWantedLevel(*pWantedLevelBaseAddress, &wantedLevel);
	hookPedestrian(*pPlayerPedBaseAddress, &player);
	cheats[0].enable();
	cheats[1].enable();
	cheats[2].enable();
}

void hack() {
	unsigned int vehicleBaseAddress = *pPlayerVehicleBaseAddress;
	if (vehicleBaseAddress) {
		hookVehicle(vehicleBaseAddress, &vehicle);
	} else {
		vehicle.baseAddress = NULL;
	}
	for (Cheat* cheat = cheats; *cheat; cheat++) {
		(*void)() onFrame = cheat->onFrame;
		if (onFrame) {
			onFrame();
		}
	}
}

void exit() {

}

void noWantedLevel() {
	*wantedLevel.heat  = 0;
	*wantedLevel.stars = 0;
}

void infiniteHealth() {
	*player.health = 999f;
	*player.armor  = 999f;
	*player.oxygen = 999f;
}

void infiniteHealthOff() {
	*player.health = 100f;
	*player.armor  = 100f;
	*player.oxygen = 100f;
}

void infiniteCarHealth() {
	if (!vehicle.baseAddress) {
		return;
	}
	*vehicle.health = 10000f;
}

void hoverCar() {
	if (!vehicle.baseAddress) {
		return;
	}
}