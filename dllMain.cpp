#include <d3d9.h>
#include <d3dx9core.h>
#include <math.h>

#include <string> 

#include "GTA.h"
#include "Cheat.h"
#include "CheatMenu.h"
#include "GTACheatMenu.h"
#include "GTASACheats.h"
#include "Memory.h"

#include "dllMain.h"

using namespace std;

/*
$(DXSDK_DIR)Include
$(DXSDK_DIR)Lib\x86

*/

/*
	Menu improvements:
	* Allow menu items to be disabled
	* Allow menu to scroll if too big (dynamically)
	* Allow menu to be left/right/center aligned, top/bottom/center aligned
	
	Ideas:
	Player movement speed
	Gravity 543087 gravity application instruction
	Player jump size (Moon jump) +0x8c, lower = higher jump
	Vehicle mass
	Car speed

	Disable collision
	Fire rockets/drop grenades from car

*/

const void (*fireRocket)(DWORD, DWORD, float, float, float, DWORD, DWORD, DWORD) = (const void(*)(DWORD, DWORD, float, float, float, DWORD, DWORD, DWORD))0x737C80;

bool* menuShowing = reinterpret_cast<bool*>(0x00BAA474);

const void(*detouredFunction)() = (const void(*)()) GAME_LOOP_FUNCTION_ADDRESS;

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
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)d3d9hookinit, 0, 0, 0);
	}
	return TRUE;
}

WantedLevel wantedLevel;
Pedestrian player;
Vehicle vehicle;

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
	new SelfDestructMenuItem()
};
const int numQuickMenuItems = sizeof(menuItems) / sizeof(CheatMenuItem*);

ActiveCheatMenuItem* mainMenuItems[] = {
	new ActiveCheatMenuItem(nullptr, &disableHoverCar, &hoverCar, "Hover Car", false),
	new ActiveCheatMenuItem(nullptr, nullptr, &bToPunt, "Press B to Punt", false),
	new ActiveCheatMenuItem(nullptr, nullptr, &rhinoCar, "Rhino Car", false),
	new ActiveCheatMenuItem(nullptr, nullptr, &holdRBForAirBrake, "Hold RB for Air Brake", false),
	new ActiveCheatMenuItem(nullptr, nullptr, &repellantTouch, "Repellant Touch", false),
	new ActiveCheatMenuItem(nullptr, nullptr, &driveOnWalls, "Drive On Walls", false),
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
	new ActiveCheatMenuItem(&enableExploreAnywhere, &disableExploreAnywhere, nullptr, "Explore Anywhere BROKEN", false),
	new ActiveCheatMenuItem(nullptr, nullptr, &dpadLeftToToggleCarLock, "D-pad Left Toggles Door Lock", true),
	new ActiveCheatMenuItem(nullptr, nullptr, &autoLockCarDoors, "Automatically Lock Doors", true),
	new ActiveCheatMenuItem(nullptr, nullptr, &discoMode, "Disco Mode", false),
};
const int numMainMenuItems = sizeof(mainMenuItems) / sizeof(ActiveCheatMenuItem*);

GTASACheatMenu quickMenu = GTASACheatMenu(menuItems, sizeof(menuItems) / sizeof(CheatMenuItem*));
GTASACheatMenu mainMenu = GTASACheatMenu((CheatMenuItem**)mainMenuItems, numMainMenuItems);

DWORD preDetourFunctionCall;

void init() {
	hookWantedLevel(*pWantedLevelBaseAddress, &wantedLevel);
	hookPedestrian(*pPlayerPedBaseAddress, &player);

	DWORD hookOffset = (DWORD)&detour - GAME_LOOP_FUNCTION_CALL - 4;
	overwriteInstructions((void*)GAME_LOOP_FUNCTION_CALL, &hookOffset, 4, &preDetourFunctionCall);
	displayMessage("~p~Cheat Device~w~ loaded~N~Hit LB + dpad for menu", 0, 0, 0);
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
DWORD vehicleBALastFrame = NULL;

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
	previousButtonState = gamepadState.Gamepad.wButtons;
	if (buttonsHeld & XINPUT_GAMEPAD_LEFT_SHOULDER) {
		if (displayMenu) {
			if (buttonsPressed & XINPUT_GAMEPAD_DPAD_UP) {
				mainMenu.menuUp();
			} else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_DOWN) {
				mainMenu.menuDown();
			}
			else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_LEFT) {
				mainMenu.selectedMenuItemLeft();
			}
			else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_RIGHT) {
				mainMenu.selectedMenuItemRight();
			}
			else if (buttonsPressed & XINPUT_GAMEPAD_A) {
				mainMenu.activateSelectedMenuItem();
			}
			else if (buttonsPressed & XINPUT_GAMEPAD_B) {
				displayMenu = false;
			}
		}
		else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_UP || buttonsPressed & XINPUT_GAMEPAD_DPAD_DOWN ||
			buttonsPressed & XINPUT_GAMEPAD_DPAD_LEFT || buttonsPressed & XINPUT_GAMEPAD_DPAD_RIGHT) {
			displayMenu = true;
			displayQuickMenu = false;
		}
	}
	previousButtonState = gamepadState.Gamepad.wButtons;
	if (buttonsHeld & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		if (displayQuickMenu) {
			if (buttonsPressed & XINPUT_GAMEPAD_DPAD_UP) {
				quickMenu.menuUp();
			}
			else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_DOWN) {
				quickMenu.menuDown();
			}
			else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_LEFT) {
				quickMenu.selectedMenuItemLeft();
			}
			else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_RIGHT) {
				quickMenu.selectedMenuItemRight();
			}
			else if (buttonsPressed & XINPUT_GAMEPAD_A) {
				quickMenu.activateSelectedMenuItem();
			}
			else if (buttonsPressed & XINPUT_GAMEPAD_B) {
				displayQuickMenu = false;
			}
		}
		else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_UP || buttonsPressed & XINPUT_GAMEPAD_DPAD_DOWN ||
			buttonsPressed & XINPUT_GAMEPAD_DPAD_LEFT || buttonsPressed & XINPUT_GAMEPAD_DPAD_RIGHT) {
			displayQuickMenu = true;
			displayMenu = false;
		}
	}
	vehicleBALastFrame = vehicleBaseAddress;
}


DWORD endSceneAddress, jumpBackAddress;
void (*EndScene)(LPDIRECT3DDEVICE9);
byte overwrittenEndSceneCode[5];

void d3d9hookinit() {
	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d) {
		return;
	}
	IDirect3DDevice9* dummyDevice = nullptr;
	D3DPRESENT_PARAMETERS params = {};
	params.Windowed = false;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	HWND handle = FindWindow(NULL, "GTA: San Andreas");
	params.hDeviceWindow = handle;

	HRESULT success = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, params.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &dummyDevice);
	if (success != S_OK) {
		params.Windowed = true;
		success = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, params.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &dummyDevice);
	}
	d3d->Release();
	if (success != S_OK) {
		return;
	}
	DWORD** vtable = reinterpret_cast<DWORD**>(dummyDevice);
	endSceneAddress = (*vtable)[42];
	jumpBackAddress = endSceneAddress + 6;
	EndScene = (void(*)(LPDIRECT3DDEVICE9))endSceneAddress;
	/*ofstream f;
	f.open("endscene.txt");
	f << hex << endSceneAddress << endl << (*vtable + 42);
	f.close();*/
	byte jump[5] = { 0xe9, 0, 0, 0, 0 };
	DWORD jumpOffset = (DWORD)&endSceneDetour - endSceneAddress + FAR_JUMP_OFFSET + 2;
	memcpy(&jump[1], &jumpOffset, 4);
	overwriteInstructions((void*)endSceneAddress, jump, 5, overwrittenEndSceneCode);
	dummyDevice->Release();
}

LPDIRECT3DDEVICE9 pDevice;
LPD3DXFONT pFont = NULL;
LPD3DXSPRITE pSprite;
D3DVIEWPORT9 pViewport;
int fontSize = 0;
int lineGap = 0;

void APIENTRY drawScene() {
	if (!pFont) {
		pDevice->GetViewport(&pViewport);
		int height = pViewport.Height;
		fontSize = (int)ceil(.03 * height);
		lineGap = (int)ceil(.25 * fontSize);
		lineGap = 0;
		D3DXCreateFont(pDevice, fontSize, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Franklin Gothic"), &pFont);
		D3DXCreateSprite(pDevice, &pSprite);
	}
	pSprite->Begin(D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_ALPHABLEND);
	if (displayMenu) {
		mainMenu.show(pDevice, pFont, pSprite);
	}
	if (displayQuickMenu) {
		quickMenu.show(pDevice, pFont, pSprite);
	}
	pSprite->End();
}

void endSceneDetour() {
	__asm {
		push ebp
		push esp
	}

	__asm mov pDevice, eax
	drawScene();
	__asm {
		pop esp
		pop ebp
		push ebp
		mov ebp, esp
		mov eax, [ebp + 0x08]
		jmp jumpBackAddress
	}
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
	restoreInstructions((void*)endSceneAddress, overwrittenEndSceneCode, sizeof(overwrittenEndSceneCode));
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