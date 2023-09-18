#include <Windows.h>

#include "GTA.h"
#include "Cheat.h"
#include "dllMain.h"

#define GAME_LOOP_FUNCTION_CALL		0x0053C096
#define GAME_LOOP_FUNCTION_ADDRESS	0x005684A0

const void(*gameLoop)() = (const void(*)()) GAME_LOOP_FUNCTION_ADDRESS;

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

Cheat* cheats[] = {
	new Cheat(nullptr, nullptr, &noWantedLevel),
	new Cheat(nullptr, &infiniteHealthOff, &infiniteHealth),
	new Cheat(nullptr, nullptr, &infiniteCarHealth),
};
const int numCheats = sizeof(cheats) / sizeof(Cheat*);

unsigned int originalFunctionCall;
void init() {
	hookWantedLevel(*pWantedLevelBaseAddress, &wantedLevel);
	hookPedestrian(*pPlayerPedBaseAddress, &player);
	cheats[0]->setEnabled(true);
	cheats[1]->setEnabled(true);
	cheats[2]->setEnabled(true);

	DWORD oldPolicy;
	unsigned int hookOffset = (unsigned int)&detour - GAME_LOOP_FUNCTION_CALL - 4;

	VirtualProtect((LPVOID)GAME_LOOP_FUNCTION_CALL, 4, PAGE_EXECUTE_READWRITE, &oldPolicy);
	originalFunctionCall = *(unsigned int*)GAME_LOOP_FUNCTION_CALL;
	*(unsigned int*)GAME_LOOP_FUNCTION_CALL = hookOffset;
	VirtualProtect((LPVOID)GAME_LOOP_FUNCTION_CALL, 4, oldPolicy, &oldPolicy);
}

void detour()
{
	gameLoop();
	hack();
}

void hack() {
	unsigned int vehicleBaseAddress = *pPlayerVehicleBaseAddress;
	if (vehicleBaseAddress) {
		hookVehicle(vehicleBaseAddress, &vehicle);
	} else {
		vehicle.baseAddress = NULL;
	}
	for (int i = 0; i < numCheats; i++) {
		void(*onFrame)() = cheats[i]->onFrame;
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
	*player.health = 999.f;
	*player.armor  = 999.f;
	*player.oxygen = 999.f;
}

void infiniteHealthOff() {
	*player.health = 100.f;
	*player.armor  = 100.f;
	*player.oxygen = 100.f;
}

void infiniteCarHealth() {
	if (!vehicle.baseAddress) {
		return;
	}
	*vehicle.health = 10000.f;
}

void hoverCar() {
	if (!vehicle.baseAddress) {
		return;
	}
}