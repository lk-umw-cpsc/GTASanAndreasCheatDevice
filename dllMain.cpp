#include <d3d9.h>
#include <d3dx9core.h>
#include <math.h>

#include <string>
#include <unordered_map>
#include <chrono>

#include "GTA.h"
#include "Cheat.h"
#include "CheatMenu.h"
#include "GTACheatMenu.h"
#include "GTASACheats.h"
#include "Memory.h"
#include "D3D9EndSceneHook.h"

#include "dllMain.h"

using namespace std;

/*
$(DXSDK_DIR)Include
$(DXSDK_DIR)Lib\x86

*/

/*
	Menu improvements:
	* Allow menu to scroll if too big (dynamically)
	* Input handler class
	* Move D3D hook etc to own file
	* Info on screen (coords, etc) w/ LB+RB
	
	Ideas:
	Don't lose weapons on death
	Player movement speed
	Player jump size (Moon jump) +0x8c, lower = higher jump
	Vehicle mass
	Car speed

	Disable collision
	Fire rockets/drop grenades from car

*/

const void (*fireRocket)(DWORD, DWORD, float, float, float, DWORD, DWORD, DWORD) = (const void(*)(DWORD, DWORD, float, float, float, DWORD, DWORD, DWORD))0x737C80;

const voidFunction detouredFunction = (const voidFunction) GAME_LOOP_FUNCTION_ADDRESS;

bool displayMenu, displayQuickMenu;

HWND hwnd;
HINSTANCE hDLL;

// Function called by Windows, including on DLL "entry point" once it loads within our target process
BOOL WINAPI DllMain(__in  HINSTANCE hinstDLL, __in  DWORD fdwReason, __in  LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// Call the init function once the DLL is attached to a process
		hDLL = hinstDLL;
		init();
		installD3D9Hook((char*)"GTA: San Andreas", drawScene);
	}
	return TRUE;
}

WantedLevel wantedLevel;
Pedestrian player;
Vehicle vehicle;

BYTE originalBunnyHopInstructions[2];

CheatMenuItem* menuItems[] = {
	new SpawnCarMenuItem(506),
	new RepairVehicleMenuItem(),
	new KillEveryoneMenuItem(),
	new BlowUpAllVehicles(),
	new VehicleColorMenuItem(),
	new QuickTakeOffMenuItem(),
	new ChangeGravityMenuItem(),
	new StepForwardMenuItem(),
	new StepUpMenuItem(),
	new StepDownMenuItem(),
	new BunnyHopMenuItem(),
	new SelfDestructMenuItem()
};
const int numQuickMenuItems = sizeof(menuItems) / sizeof(CheatMenuItem*);

ActiveCheatMenuItem* mainMenuItems[] = {
	new ActiveCheatMenuItem(nullptr, &disableHoverCar, &hoverCar, "Hover Car", false),
	new ActiveCheatMenuItem(nullptr, nullptr, &bToPunt, "Press B to Punt", false),
	new ActiveCheatMenuItem(nullptr, nullptr, &rhinoCar, "Rhino Car", false),
	new ActiveCheatMenuItem(nullptr, nullptr, &holdRBForAirBrake, "Hold RB for Air Brake", false),
	new ActiveCheatMenuItem(nullptr, nullptr, &repellantTouch, "Repellant Touch", false),
	new ActiveCheatMenuItem(nullptr, nullptr, &driveOnWalls, "Drive on Walls", false),
	new ActiveCheatMenuItem(nullptr, nullptr, &infiniteNos, "Infinite NOS", false),
	new ActiveCheatMenuItem(nullptr, &disableNoWantedLevel, &noWantedLevel, "No Wanted Level", true),
	new ActiveCheatMenuItem(nullptr, &infiniteHealthOff, &infiniteHealth, "Infinite Health & Armor", true),
	new ActiveCheatMenuItem(&enableNoPlaneDamage, &disableNoPlaneDamage, &infiniteCarHealth, "Indestructible Vehicle", true),
	new ActiveCheatMenuItem(&installNoCarDamageDetour, &uninstallNoCarDamageDetour, &fixCar, "No Cosmetic Damage", true),
	new ActiveCheatMenuItem(nullptr, nullptr, &autoFlipCar, "Automatically Flip Car", true),
	new ActiveCheatMenuItem(&installFallOffBikeDetour, &uninstallFallOffBikeDetour, nullptr, "Never Fall Off Bike", true),
	new ActiveCheatMenuItem(&enableInfiniteAmmo, &disableInfiniteAmmo, nullptr, "Infinite Ammo", true),
	new ActiveCheatMenuItem(&enableEnterAnyVehicle, &disableEnterAnyVehicle, nullptr, "Enter Any Vehicle", true),
	new ActiveCheatMenuItem(&installPropertyPurchaseDetour, &uninstallPropertyPurchaseDetour, nullptr, "Purchase Any Property", true),
	//new ActiveCheatMenuItem(&enableExploreAnywhere, &disableExploreAnywhere, nullptr, "Explore Anywhere BROKEN", false),
	new ActiveCheatMenuItem(nullptr, nullptr, &dpadLeftToToggleCarLock, "D-pad Left Toggles Door Lock", true),
	new ActiveCheatMenuItem(nullptr, nullptr, &autoLockCarDoors, "Automatically Lock Doors", true),
	new ActiveCheatMenuItem(nullptr, nullptr, &discoMode, "Disco Mode", false),
	new ActiveCheatMenuItem(&enableTrippy, &disableTrippy, nullptr, "Far Out! Mode", false),
	new ActiveCheatMenuItem(nullptr, nullptr, spawnExplosionAtTargetPedestrian, "D-pad Right Blows Up Pedestrian", false),
	new ActiveCheatMenuItem(nullptr, nullptr, goAway, "Go Away!", false),
};
const int numMainMenuItems = sizeof(mainMenuItems) / sizeof(ActiveCheatMenuItem*);

CheatMenuControls mainMenuControls = {
	createHotKeys("hold LB and press up"),
	createHotKeys("hold LB and press down"),
	createHotKeys("hold LB and press left"),
	createHotKeys("hold LB and press right"),
	createHotKeys("hold LB and press A"),
	createHotKeys("hold LB and press B or hold RB and press up or hold RB and press down or hold RB and press left or hold RB and press right"),
};

MenuStyle mainMenuStyle = {
	HORIZONTAL_ANCHOR_LEFT,
	VERTICAL_ANCHOR_TOP,
	.02,
	.02f,
	.01f,
	.02f,
	.025f,
	.2f,
};

CheatMenuControls quickMenuControls = {
	createHotKeys("hold RB and press up"),
	createHotKeys("hold RB and press down"),
	createHotKeys("hold RB and press left"),
	createHotKeys("hold RB and press right"),
	createHotKeys("hold RB and press A"),
	createHotKeys("hold RB and press B or hold LB and press up or hold LB and press down or hold LB and press left or hold LB and press right"),
};

MenuStyle quickMenuStyle = {
	HORIZONTAL_ANCHOR_RIGHT,
	VERTICAL_ANCHOR_CENTER,
	.02,
	.02f,
	.01f,
	.02f,
	.025f,
	.2f
};

GTASACheatMenu quickMenu = GTASACheatMenu(menuItems, sizeof(menuItems) / sizeof(CheatMenuItem*), &quickMenuStyle, &quickMenuControls);
GTASACheatMenu mainMenu = GTASACheatMenu((CheatMenuItem**)mainMenuItems, numMainMenuItems, &mainMenuStyle, &mainMenuControls);

DWORD preDetourFunctionCall;

void init() {
	hookWantedLevel(*pWantedLevelBaseAddress, &wantedLevel);
	hookPedestrian(*pPlayerPedBaseAddress, &player);

	DWORD hookOffset = (DWORD)&detour - GAME_LOOP_FUNCTION_CALL - 4;
	overwriteInstructions((void*)GAME_LOOP_FUNCTION_CALL, &hookOffset, 4, &preDetourFunctionCall);
	displayMessage("~p~Cheat Device~w~ loaded~N~Hit LB + dpad for menu", 0, 0, 0);

	BYTE nops[2] = { 0x90, 0x90 };
	overwriteInstructions((void*)JUMP_OVER_BHMULTIPLIER_INSTRUCTION_ADDRESS, nops, sizeof(nops), originalBunnyHopInstructions);
}

void detour()
{
	detouredFunction();
	hack();
}

bool usingGamepad;
XINPUT_STATE gamepadState;
WORD buttonsHeld;
WORD buttonsPressed;
WORD buttonsReleased;
WORD previousButtonState;
WORD carSpawnID = 506; // Super GT
DWORD vehicleBaseAddressLastFrame = NULL;

typedef struct Repeat {
	long long timer;
	long long nextRepeat;
};
unordered_map<WORD, Repeat> buttonRepeatTimers;

DWORD getRepeats(DWORD buttonsHeld) {
	DWORD repeats = 0;
	using namespace std::chrono;
	long long now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	for (int button = 1; button; button <<= 1) {
		if (buttonsHeld & button) {
			if (buttonRepeatTimers.find(button) != buttonRepeatTimers.end()) {
				Repeat repeat = buttonRepeatTimers[button];
				if (repeat.timer < now) {
					repeats |= button;
					repeat.timer = now + repeat.nextRepeat;
					repeat.nextRepeat = max(repeat.nextRepeat - 50, 100);
					buttonRepeatTimers[button] = repeat;
				}
			}
			else {
				Repeat repeat = { now + 300, 250 };
				buttonRepeatTimers[button] = repeat;
			}
		}
		else {
			buttonRepeatTimers.erase(button);
		}
	}
	return repeats;
}

void hack() {
	if (GetAsyncKeyState(VK_DELETE) & KEY_PRESSED_MASK) {
		exit();
		return;
	}
	DWORD playerBaseAddress = *pPlayerPedBaseAddress;
	DWORD vehicleBaseAddress = *pPlayerVehicleBaseAddress;
	if (playerBaseAddress != player.baseAddress && playerBaseAddress) {
		hookPedestrian(playerBaseAddress, &player);
	}
	if (vehicleBaseAddress) {
		hookVehicle(vehicleBaseAddress, &vehicle);
	} else {
		vehicle.baseAddress = NULL;
	}
	//if (GetAsyncKeyState(VK_NUMPAD2) & KEY_PRESSED_MASK) {
	//	// 741a53
	//	fireRocket(player.baseAddress, 19, player.position->x + player.forward->x * 5, player.position->y + player.forward->y * 5, player.position->z + player.forward->z * 5, 0, 0, 0);
	//}
	memset(&gamepadState, 0, sizeof(XINPUT_STATE));
	usingGamepad = XInputGetState(0, &gamepadState) == ERROR_SUCCESS;
	buttonsHeld = gamepadState.Gamepad.wButtons;
	buttonsPressed = (~previousButtonState) & gamepadState.Gamepad.wButtons;
	buttonsPressed |= getRepeats(buttonsHeld);
	buttonsReleased = previousButtonState & (~gamepadState.Gamepad.wButtons);
	for (int i = 0; i < numMainMenuItems; i++) {
		ActiveCheatMenuItem* menuItem = (ActiveCheatMenuItem*)mainMenu.getMenuItem(i);
		if (menuItem->isEnabled()) {
			voidFunction onFrame = menuItem->onFrame;
			if (onFrame) {
				onFrame();
			}
		}
	}

	mainMenu.handleInput(buttonsHeld, buttonsPressed, buttonsReleased);
	quickMenu.handleInput(buttonsHeld, buttonsPressed, buttonsReleased);

	previousButtonState = gamepadState.Gamepad.wButtons;
	vehicleBaseAddressLastFrame = vehicleBaseAddress;
}

void exit() {
	for (int i = 0; i < numMainMenuItems; i++) {
		ActiveCheatMenuItem* menuItem = (ActiveCheatMenuItem*)mainMenu.getMenuItem(i);
		menuItem->setEnabled(false);
		delete menuItem;
	}
	for (int i = 0; i < numQuickMenuItems; i++) {
		delete quickMenu.getMenuItem(i);
	}
	restoreInstructions((void*)GAME_LOOP_FUNCTION_CALL, &preDetourFunctionCall, 4);
	uninstallD3D9Hook();
	quickMenu.releaseD3DObjects();
	mainMenu.releaseD3DObjects();
	restoreInstructions((void*)JUMP_OVER_BHMULTIPLIER_INSTRUCTION_ADDRESS, originalBunnyHopInstructions, sizeof(originalBunnyHopInstructions));

	displayMessage("Cheat device unloaded", 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)unload, 0, 0, 0);
}

void unload() {
	HMODULE h;
	char name[MAX_PATH];
	GetModuleFileName(hDLL, name, MAX_PATH);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, name, &h);
	FreeLibraryAndExitThread(hDLL, 0);
}

void APIENTRY drawScene(LPDIRECT3DDEVICE9 pDevice) {
	if (*gamePaused) {
		return;
	}
	mainMenu.show(pDevice);
	quickMenu.show(pDevice);
}