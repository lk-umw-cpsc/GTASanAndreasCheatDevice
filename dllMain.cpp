#include <Windows.h>
#include <d3d9.h>
#include <d3dx9core.h>
#include <string>
#include <math.h>
#include <Xinput.h>
#include <fstream>

#include "GTA.h"
#include "Cheat.h"
#include "dllMain.h"
#include "Memory.h"
#include "QuickMenu.h"
#include "GTAQuickMenu.h"

using namespace std;

/*
$(DXSDK_DIR)Include
$(DXSDK_DIR)Lib\x86

*/

/*
	Ideas:
	Player movement speed
	Gravity 543087 gravity application instruction
	Player jump size (Moon jump) +0x8c, lower = higher jump
	Punt
	Vehicle mass
	Car speed
	Crimes
	Text? gta_sa.exe+31A243 
*/

// derived from 5-byte JMP far opcode, -1 because of push esi created by Visual Studio
#define FAR_JUMP_OFFSET (-4)

#define GAME_LOOP_FUNCTION_CALL				0x0053C096
#define GAME_LOOP_FUNCTION_ADDRESS			0x005684A0

// 0x006CCCF0 0xC2, 0x08 - Disable plane explosions upon crashing

const void (*fireRocket)(unsigned int, unsigned int, float, float, float, unsigned int, unsigned int, unsigned int) = (const void(*)(unsigned int, unsigned int, float, float, float, unsigned int, unsigned int, unsigned int))0x737C80;

bool* menuShowing = reinterpret_cast<bool*>(0x00BAA474);

#define KEY_PRESSED_MASK 0x1
#define KEY_DOWN_MASK 0x8000

#define distance(a, b, c) ((float)sqrt(a*a + b*b + c*c))
#define distance2d(a, b) ((float)sqrt(a*a + b*b))

const void(*gameLoop)() = (const void(*)()) GAME_LOOP_FUNCTION_ADDRESS;

bool displayMenu;

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

QuickMenuItem* menuItems[] = {
	new SpawnCarMenuItem(506),
	new RepairVehicleMenuItem(),
	new BlowUpMenuItem()
};

GTASAQuickMenu quickMenu = GTASAQuickMenu(menuItems, sizeof(menuItems) / sizeof(QuickMenuItem*));

Cheat* cheats[] = {
	// onEnable, onDisable, onFrame
	new Cheat(nullptr, &disableHoverCar, &hoverCar, "Hover Car", false),
	//new Cheat(nullptr, nullptr, &bToPunt, "Press B to punt", false),
	new Cheat(nullptr, nullptr, &rhinoCar, "Rhino Car", false),
	new Cheat(nullptr, &disableNoWantedLevel, &noWantedLevel, "No Wanted Level", true),
	new Cheat(nullptr, &infiniteHealthOff, &infiniteHealth, "Infinite Health", true),
	new Cheat(nullptr, nullptr, &infiniteCarHealth, "Indestructible Vehicle", true),
	new Cheat(nullptr, nullptr, &autoFlipCar, "Auto Flip Car", true),
	new Cheat(&installFallOffBikeDetour, &uninstallFallOffBikeDetour, nullptr, "Never Fall Off Bike", true),
	new Cheat(&enableInfiniteAmmo, &disableInfiniteAmmo, nullptr, "Infinite Ammo", true),
	new Cheat(&installNoCarDamageDetour, &uninstallNoCarDamageDetour, nullptr, "No Cosmetic Damage", true),
	new Cheat(&enableEnterAnyVehicle, &disableEnterAnyVehicle, nullptr, "Enter Any Vehicle", true),
	new Cheat(&installPropertyPurchaseDetour, &uninstallPropertyPurchaseDetour, nullptr, "Purchase Any Property", true),
	new Cheat(&enableExploreAnywhere, &disableExploreAnywhere, nullptr, "Explore Anywhere", true),
	//new Cheat(&enableNoPlaneExplosion, &disableNoPlaneExplosion, nullptr, "No Plane Explosion"),
};
const int numCheats = sizeof(cheats) / sizeof(Cheat*);
int menuIndex = 0;

unsigned int originalFunctionCall;
void init() {
	hookWantedLevel(*pWantedLevelBaseAddress, &wantedLevel);
	hookPedestrian(*pPlayerPedBaseAddress, &player);

	DWORD oldPolicy;
	unsigned int hookOffset = (unsigned int)&detour - GAME_LOOP_FUNCTION_CALL - 4;
	overwriteInstructions((void*)GAME_LOOP_FUNCTION_CALL, &hookOffset, 4, &originalFunctionCall);
	displayMessage("~p~Cheat Device~w~ loaded~N~Hit LB + dpad for menu", 0, 0, 0);
}

void detour()
{
	gameLoop();
	hack();
}

bool usingGamepad;
XINPUT_STATE gamepadState;
WORD buttonsHeld;
WORD buttonsPressed;
WORD buttonsReleased;
WORD previousButtonState;
WORD carSpawnID = 506; // Super GT
unsigned int vehicleBALastFrame = NULL;

void hack() {
	if (GetAsyncKeyState(VK_DELETE) & KEY_PRESSED_MASK) {
		exit();
		return;
	}
	unsigned int vehicleBaseAddress = *pPlayerVehicleBaseAddress;
	if (vehicleBaseAddress) {
		hookVehicle(vehicleBaseAddress, &vehicle);
		if (vehicleBaseAddress != vehicleBALastFrame) {
			// in a new car
			*vehicle.lock = VEHICLE_LOCK_STATE_LOCKED;
		}
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
	for (int i = 0; i < numCheats; i++) {
		if (cheats[i]->isEnabled()) {
			void(*onFrame)() = cheats[i]->onFrame;
			if (onFrame) {
				onFrame();
			}
		}
	}
	previousButtonState = gamepadState.Gamepad.wButtons;
	if (buttonsHeld & XINPUT_GAMEPAD_LEFT_SHOULDER) {
		if (displayMenu) {
			if (buttonsPressed & XINPUT_GAMEPAD_DPAD_UP) {
				menuIndex = (menuIndex + numCheats - 1) % numCheats;
				//showMenu();
			} else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_DOWN) {
				menuIndex = (menuIndex + 1) % numCheats;
				//showMenu();
			} else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_LEFT || buttonsPressed & XINPUT_GAMEPAD_DPAD_RIGHT) {
				cheats[menuIndex]->toggle();
				//showMenu();
			}
			else if (buttonsPressed & XINPUT_GAMEPAD_B) {
				displayMenu = false;
			}
		}
		else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_UP || buttonsPressed & XINPUT_GAMEPAD_DPAD_DOWN ||
			buttonsPressed & XINPUT_GAMEPAD_DPAD_LEFT || buttonsPressed & XINPUT_GAMEPAD_DPAD_RIGHT) {
			//showMenu();
			displayMenu = true;
		}
	}
	previousButtonState = gamepadState.Gamepad.wButtons;
	if (buttonsHeld & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		if (*menuShowing) {
			if (buttonsPressed & XINPUT_GAMEPAD_DPAD_UP) {
				quickMenu.menuUp();
				quickMenu.show();
			}
			else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_DOWN) {
				quickMenu.menuDown();
				quickMenu.show();

			}
			else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_LEFT) {
				quickMenu.selectedMenuItemLeft();
				quickMenu.show();
			}
			else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_RIGHT) {
				quickMenu.selectedMenuItemRight();
				quickMenu.show();
			}
			else if (buttonsPressed & XINPUT_GAMEPAD_A) {
				quickMenu.activateSelectedMenuItem();
				quickMenu.show();
			}
		}
		else if (buttonsPressed & XINPUT_GAMEPAD_DPAD_UP || buttonsPressed & XINPUT_GAMEPAD_DPAD_DOWN ||
			buttonsPressed & XINPUT_GAMEPAD_DPAD_LEFT || buttonsPressed & XINPUT_GAMEPAD_DPAD_RIGHT) {
			quickMenu.show();
		}
	}
	vehicleBALastFrame = vehicleBaseAddress;
}


unsigned int endSceneAddress, jumpBackAddress;
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
	unsigned int** vtable = reinterpret_cast<unsigned int**>(dummyDevice);
	endSceneAddress = (*vtable)[42];
	jumpBackAddress = endSceneAddress + 6;
	EndScene = (void(*)(LPDIRECT3DDEVICE9))endSceneAddress;
	ofstream f;
	f.open("endscene.txt");
	f << hex << endSceneAddress << endl << (*vtable + 42);
	f.close();
	/*DWORD oldProtectionFlags;
	VirtualProtect((void*)(*vtable + 42), 4, PAGE_EXECUTE_READWRITE, &oldProtectionFlags);
	(*vtable)[42] = (DWORD)endSceneDetour;
	VirtualProtect((void*)(*vtable + 42), 4, oldProtectionFlags, &oldProtectionFlags);*/
	byte jump[5] = { 0xe9, 0, 0, 0, 0 };
	DWORD jumpOffset = (DWORD)&endSceneDetour - endSceneAddress + FAR_JUMP_OFFSET + 2;
	memcpy(&jump[1], &jumpOffset, 4);
	overwriteInstructions((void*)endSceneAddress, jump, 5, overwrittenEndSceneCode);
}
LPDIRECT3DDEVICE9 pDevice;
LPD3DXFONT pFont = NULL;
LPD3DXSPRITE pSprite;
// __fastcall 
void APIENTRY drawScene() {
	if (!displayMenu) {
		return;
	}
	//D3DRECT r = { 10, 10, 20, 20 };
	RECT r = { 100, 100, 1000, 300 };
	D3DCOLOR color = D3DCOLOR_ARGB(100, 255, 0, 0);
	D3DCOLOR color2 = D3DCOLOR_ARGB(255, 255, 255, 255);
	if (!pFont) {
		D3DXCreateFont(pDevice, 64, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &pFont);
		D3DXCreateSprite(pDevice, &pSprite);
	}
	//D3DXSPRITE_BILLBOARD | 
	pSprite->Begin(D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_ALPHABLEND);
	//pFont->DrawText(pSprite, "Hello, world!", -1, &r, DT_LEFT, color);
	string text = "";
	bool selectedCheat;
	D3DCOLOR* chosenColor;
	for (int i = 0; i < numCheats; i++) {
		selectedCheat = i == menuIndex;
		if (selectedCheat) {
			chosenColor = &color2;
		}
		else {
			chosenColor = &color;
		}
		r.top += 64 + 12;
		r.bottom += 64 + 12;
		TEXTMETRIC metric;
		pFont->GetTextMetrics(&metric);
		metric.
		pFont->DrawText(pSprite, cheats[i]->getMenuText().c_str(), -1, &r, DT_LEFT, *chosenColor);
	}
	pSprite->End();
	//pDevice->Clear(1, &r, D3DCLEAR_TARGET, color, 0, 0);
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
	for (int i = 0; i < numCheats; i++) {
		cheats[i]->setEnabled(false);
	}
	restoreInstructions((void*)GAME_LOOP_FUNCTION_CALL, &originalFunctionCall, 4);
	restoreInstructions((void*)endSceneAddress, overwrittenEndSceneCode, sizeof(overwrittenEndSceneCode));
	displayMessage("Cheat device unloaded", 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)unload, 0, 0, 0);
}

void unload() {
	HMODULE h;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, "SADevice.dll", &h);
	FreeLibraryAndExitThread(hDLL, 0);
}

void showMenu() {
	string text = MENU_WHITE_TEXT_TOKEN;
	bool selectedCheat;
	for (int i = 0; i < numCheats; i++) {
		if (i > 0) {
			text += MENU_NEW_LINE;
		}
		selectedCheat = i == menuIndex;
		if (selectedCheat) {
			text += MENU_PURPLE_TEXT_TOKEN;
		}
		text += cheats[i]->getMenuText();
		if (selectedCheat) {
			text += MENU_WHITE_TEXT_TOKEN;
		}
	}
	displayMessage(text.c_str(), 0, 0, 0);
}

void noWantedLevel() {
	*pNoCrimesFlag = 1;
	*wantedLevel.heat  = 0;
	*wantedLevel.stars = 0;
}

void disableNoWantedLevel() {
	*pNoCrimesFlag = 0;
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

#define HOVER_CAR_MAX_VELOCITY 3.0f
#define HOVER_CAR_ACCELERATION 0.1f
#define HOVER_CAR_FRICTION 0.1f
#define HOVER_CAR_FORWARD_FORCE (1.0f - HOVER_CAR_FRICTION)
void hoverCar() {
	if (!vehicle.baseAddress) {
		return;
	}
	if (vehicle.objectClass == CLASS_CAR) {
		float tireRot = *vehicle.tireRotation;
		if (tireRot < 2.0f) {
			*vehicle.tireRotation = tireRot + 0.05f;
		}
		else if (tireRot > 2.0f) {
			*vehicle.tireRotation = 2.0f;
		}
	}
	bool accelerate_button_held;
	bool reverse_button_held;
	bool lift_button_held;
	bool punt_button_pressed;
	bool hover_car_toggle_button_pressed;
	bool rocket_button_pressed = false;
	bool rotate_car_button_held = false;
	float left_stick_x = 0.0f, left_stick_y = 0.0f;
	float right_stick_x = 0.0f, right_stick_y = 0.0f;
	if (usingGamepad) {
		accelerate_button_held = buttonsHeld & XINPUT_GAMEPAD_A;
		reverse_button_held = buttonsHeld & XINPUT_GAMEPAD_X;
		lift_button_held = buttonsHeld & XINPUT_GAMEPAD_B;
		punt_button_pressed = buttonsHeld & XINPUT_GAMEPAD_DPAD_UP;
		hover_car_toggle_button_pressed = buttonsPressed & XINPUT_GAMEPAD_DPAD_DOWN;
		rocket_button_pressed = buttonsPressed & XINPUT_GAMEPAD_LEFT_THUMB;
		rotate_car_button_held = buttonsHeld & XINPUT_GAMEPAD_RIGHT_SHOULDER;

		if (abs(gamepadState.Gamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			left_stick_x = gamepadState.Gamepad.sThumbLX / (float)(32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		if (abs(gamepadState.Gamepad.sThumbLY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			left_stick_y = gamepadState.Gamepad.sThumbLY / (float)(32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		if (abs(gamepadState.Gamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			right_stick_x = gamepadState.Gamepad.sThumbRX / (float)(32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		if (abs(gamepadState.Gamepad.sThumbRY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			right_stick_y = gamepadState.Gamepad.sThumbRY / (float)(32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	}
	else {
		return;
	}

	float rx = vehicle.forward->x;
	float ry = vehicle.forward->y;
	float rv = (float)sqrt(rx * rx + ry * ry);
	rx /= rv;
	ry /= rv;

	*vehicle.forward = { rx, ry, 0.0f };
	*vehicle.left = { ry, -rx, 0.0f };
	*vehicle.up = { 0.0f, 0.0f, 1.0f };
	vehicle.rotationalVelocity->x = 0.0f;
	vehicle.rotationalVelocity->y = 0.0f;

	if (accelerate_button_held) {
		Vector3d vehicleVelocity = *vehicle.velocity;
		float velocity = distance2d(vehicleVelocity.x, vehicleVelocity.y);
		velocity = min(velocity, HOVER_CAR_MAX_VELOCITY);
		vehicle.velocity->x = vehicle.forward->x * velocity * HOVER_CAR_FORWARD_FORCE + vehicle.velocity->x * HOVER_CAR_FRICTION + vehicle.forward->x * HOVER_CAR_ACCELERATION;
		vehicle.velocity->y = vehicle.forward->y * velocity * HOVER_CAR_FORWARD_FORCE + vehicle.velocity->y * HOVER_CAR_FRICTION + vehicle.forward->y * HOVER_CAR_ACCELERATION;
	} else if (reverse_button_held) {
		vehicle.velocity->x = vehicle.forward->x * -0.8f;
		vehicle.velocity->y = vehicle.forward->y * -0.8f;
	} else {
		vehicle.velocity->x *= 0.9f;
		vehicle.velocity->y *= 0.9f;
	}
	if (left_stick_y != 0.0f) {
		vehicle.velocity->z = 0.8f * left_stick_y;
	}
	else {
		//if (car->wheels[0] || car->wheels[1] || car->wheels[2] || car->wheels[3]) {
			//car->velocity.z = 0.019f;
		//} else {
			
		//}
		vehicle.velocity->z = 0.013f;
	}
	if (left_stick_x) {
		vehicle.rotationalVelocity->z = 0.05f * -left_stick_x;
	} else {
		vehicle.rotationalVelocity->z *= 0.5f;
	}
}

void disableHoverCar() {
	if (!vehicle.baseAddress || vehicle.objectClass != CLASS_CAR) {
		return;
	}
	*vehicle.tireRotation = 0.7046132684f;
}

void autoFlipCar() {
	if (!vehicle.baseAddress) {
		return;
	}
	if (vehicle.up->z < -0.2f) {
		if (vehicle.rotationalVelocity->y > 0)
			vehicle.rotationalVelocity->y = 0.05f;
		else
			vehicle.rotationalVelocity->y = -0.05f;
	}
}

BYTE originalFallOffBikeCall[5];
void installFallOffBikeDetour() {
	DWORD newJumpAddress;
	newJumpAddress = (DWORD)&fallOffBikeDetour - 0x4b4ac0 - 0x05;
	BYTE detour[] = { 0xE9, 0, 0, 0, 0 }; //E9 for jmp
	memcpy((LPVOID)(detour + 1), (LPVOID)&newJumpAddress, sizeof(newJumpAddress));
	overwriteInstructions((void*)0x4b4ac0, detour, sizeof(detour), originalFallOffBikeCall);
}

void fallOffBikeDetour() {
	__asm {
		push ecx
		mov ecx, 0xb6f5f0 //pplayer
		mov ecx, [ecx]
		cmp eax, ecx
		pop ecx
		jne notplayer
		mov al, 0
		ret 0004
		notplayer:
		sub esp, 0x1C
		push ebx
		push esi
		push 0x4b4ac5 // code immediately after replaced code
		ret
	}
}

void uninstallFallOffBikeDetour() {
	restoreInstructions((void*)0x4b4ac0, originalFallOffBikeCall, sizeof(originalFallOffBikeCall));
}

const DWORD infiniteAmmoNOPTargets[4] = {
	WEAPON_AMMO_DECREMENT1_ADDRESS,
	WEAPON_AMMO_DECREMENT2_ADDRESS,
	WEAPON_AMMO_DECREMENT3_ADDRESS,
	WEAPON_AMMO_DECREMENT4_ADDRESS,
};
DWORD originalAmmoDecrementCode[4][3] = { 0 };
#define NUM_AMMO_DECREMENT_OP_CODES (sizeof(infiniteAmmoNOPTargets) / sizeof(DWORD))
void enableInfiniteAmmo() {
	BYTE nops[WEAPON_AMMO_DECREMENT_SIZE] = { 0x90, 0x90, 0x90 };
	for (int i = 0; i < NUM_AMMO_DECREMENT_OP_CODES; i++) {
		overwriteInstructions((void*)infiniteAmmoNOPTargets[i], nops, WEAPON_AMMO_DECREMENT_SIZE, originalAmmoDecrementCode[i]);
	}
}

void disableInfiniteAmmo() {
	for (int i = 0; i < NUM_AMMO_DECREMENT_OP_CODES; i++) {
		restoreInstructions((void*)infiniteAmmoNOPTargets[i], originalAmmoDecrementCode[i], WEAPON_AMMO_DECREMENT_SIZE);
	}
}

BYTE originalCarDamageCall[5];
BYTE disablePlaneBlowUpInstructions[2] = { 0xeb, 0x0b };
BYTE originalPlaneBlowUpInstructions[2];
void installNoCarDamageDetour() {
	// 0x6cca2e eb 0b
	DWORD jumpDestination;
	DWORD old;
	jumpDestination = (DWORD)&noVehicleDamageDetour - 0x6A7650 - 0x04;
	//jumpDestination = (DWORD)&noVehicleDamageDetour - 0x6C24B0 - 0x04;
	BYTE detour[] = { 0xE9, 0, 0, 0, 0 }; //E9 for jmp
	memcpy((LPVOID)(detour + 1), (LPVOID)&jumpDestination, sizeof(jumpDestination));
	overwriteInstructions((void*)0x6A7650, detour, sizeof(detour), &originalCarDamageCall);
	overwriteInstructions((void*)0x6cca2e, disablePlaneBlowUpInstructions, sizeof(disablePlaneBlowUpInstructions), &originalPlaneBlowUpInstructions);
	//overwriteInstructions((void*)0x6C24B0, detour, sizeof(detour), &originalCarDamageCall);
}

void uninstallNoCarDamageDetour() {
	restoreInstructions((void*)0x6A7650, &originalCarDamageCall, sizeof(originalCarDamageCall));
	restoreInstructions((void*)0x6cca2e, &originalPlaneBlowUpInstructions, sizeof(originalPlaneBlowUpInstructions));
	//restoreInstructions((void*)0x6C24B0, &originalCarDamageCall, sizeof(originalCarDamageCall));
}

void noCarDamageDetour() {
	__asm {
		push ecx
		push ebx
		mov ecx, 0x00BA18FC
		mov ecx, [ecx]
		mov ebx, [esp + 0x0C]
		cmp ecx, ebx
		pop ebx
		pop ecx
		jne notplayercar
		ret
		notplayercar:
		push ebp
		mov ebp, [esp + 8]
		push 0x6C24B5
		ret
	}
}

void noVehicleDamageDetour() {
	// 0x6a7650
	// 6A FF 68 D8 7F 84 00
	// push -1
	// push 0x00847FD8
	__asm {
		push ebx
		mov ebx, 0x00BA18FC
		mov ebx, [ebx]
		cmp ecx, ebx
		pop ebx
		jne notpcar
		ret 0x18
	notpcar:
		push -1
		push 0x00847FD8
		push 0x6a7657
		ret
	}
}

BYTE originalCheckVehicleLockOpcodes[5];
//BYTE originalCheckVehicleLockOpcodes[2];
void enableEnterAnyVehicle() {
	//BYTE nops[2] = { 0xB0, 0x01 };
	BYTE jump[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };
	DWORD jumpOffset = (DWORD)&carLockDetour - VEHICLE_LOCK_CHECK_ADDRESS + FAR_JUMP_OFFSET;
	memcpy(&jump[1], &jumpOffset, sizeof(DWORD));
	overwriteInstructions((void*)VEHICLE_LOCK_CHECK_ADDRESS, jump, sizeof(jump), originalCheckVehicleLockOpcodes);
	//overwriteInstructions((void*)VEHICLE_LOCK_CHECK_ADDRESS, nops, sizeof(nops), originalCheckVehicleLockOpcodes);
}

void disableEnterAnyVehicle() {
	restoreInstructions((void*)VEHICLE_LOCK_CHECK_ADDRESS, originalCheckVehicleLockOpcodes, sizeof(originalCheckVehicleLockOpcodes));
}

void carLockDetour() {
	// If the driver of the car is the player, DO check the lock. Otherwise, bypass
	__asm {
		push edx
		push ecx
		add ecx, VEHICLE_PASSENGER_OFFSET
		mov ecx, [ecx]
		mov edx, PLAYER_PEDESTRIAN_STRUCT_POINTER
		mov edx, [edx]
		cmp ecx, edx
		pop ecx
		pop edx
		je driver_is_player
		mov al, 1
		pop esi
		ret 4
	driver_is_player:
		xor al, al
		pop esi
		ret 4
	}
}

BYTE noPlaneExplosionInstructions[3] = { 0xC2, 0x08, 0x00 }; // ret 08
BYTE originalPlaneExplosionInstructions[3];
BYTE noPlaneDamageOnCollisionInstructions[2] = { 0xEB, 0x0F };
BYTE originalPlaneDamageOnCollisionInstructions[2];
void enableNoPlaneExplosion() {
	//overwriteInstructions((void*)0x006CCCF0, noPlaneExplosionInstructions, sizeof(noPlaneExplosionInstructions), originalPlaneExplosionInstructions);
	overwriteInstructions((void*)0x006B1F30, noPlaneDamageOnCollisionInstructions, sizeof(noPlaneDamageOnCollisionInstructions), originalPlaneDamageOnCollisionInstructions);
}

void disableNoPlaneExplosion() {
	//restoreInstructions((void*)0x006CCCF0, originalPlaneExplosionInstructions, sizeof(originalPlaneExplosionInstructions));
	restoreInstructions((void*)0x006B1F30, originalPlaneDamageOnCollisionInstructions, sizeof(originalPlaneDamageOnCollisionInstructions));
}

void rhinoCar() {
	unsigned int vehicleBaseAddress = vehicle.baseAddress;
	if (vehicleBaseAddress) {
		unsigned int touchedObjectBaseAddress = *vehicle.pTouch;
		if (vehicleBaseAddress != vehicleBALastFrame) {
			*vehicle.mass *= 8;
		}
		if (touchedObjectBaseAddress) {
			Vehicle v;
			hookVehicle(touchedObjectBaseAddress, &v);
			if (*v.health > 0) {
				unsigned int* pTouchedObjectClass = reinterpret_cast<unsigned int*>(touchedObjectBaseAddress);
				unsigned int cls = *pTouchedObjectClass;
				switch (cls) {
				case CLASS_CAR:
				case CLASS_MOTORCYCLE:
				case CLASS_PLANE:
				case CLASS_HELICOPTER:
					unsigned int* vtable = reinterpret_cast<unsigned int*>(cls);
					unsigned int pBlowUpVehicle = vtable[41];
					void(*blowUpVehicle)(unsigned int, unsigned int) = (void(*)(unsigned int, unsigned int))(pBlowUpVehicle);
					__asm { mov ecx, [touchedObjectBaseAddress] }
					blowUpVehicle(player.baseAddress, 0);
					break;
				}
			}
		}
	}
}

Vehicle* getCurrentVehicle() {
	return &vehicle;
}

bool puntedLastFrame = false;
Vector3d pleft, pforward, pup, pposition, pvelocity, protVelocity;
#define PUNT_STEP 5.0f
void bToPunt() {
	if (vehicle.baseAddress && buttonsPressed & XINPUT_GAMEPAD_B) {
		pleft = *vehicle.left;
		pforward = *vehicle.forward;
		pup = *vehicle.up;
		pposition = *vehicle.position;
		pvelocity = *vehicle.velocity;
		protVelocity = *vehicle.rotationalVelocity;

		vehicle.velocity->x += PUNT_STEP * vehicle.forward->x;
		vehicle.velocity->y += PUNT_STEP * vehicle.forward->y;
		vehicle.velocity->z += PUNT_STEP * vehicle.forward->z;
		*vehicle.mass *= 8;
		puntedLastFrame = true;
	} else if (puntedLastFrame) {
		*vehicle.left = pleft;
		*vehicle.forward = pforward;
		*vehicle.up = pup;
		*vehicle.position = pposition;
		*vehicle.velocity = pvelocity;
		*vehicle.rotationalVelocity = protVelocity;
		*vehicle.mass /= 8;
		puntedLastFrame = false;
	}
}

byte originalPropertyPurchaseCode[5];
void installPropertyPurchaseDetour() {
	byte jump[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };
	DWORD jumpOffset = (DWORD)&propertyPurchaseDetour - 0x457dd5 + FAR_JUMP_OFFSET;
	memcpy(&jump[1], &jumpOffset, sizeof(DWORD));
	overwriteInstructions((void*)0x457dd5, jump, sizeof(jump), originalPropertyPurchaseCode);
}

void uninstallPropertyPurchaseDetour() {
	restoreInstructions((void*)0x457dd5, originalPropertyPurchaseCode, sizeof(originalPropertyPurchaseCode));
}

void propertyPurchaseDetour() {
	__asm {
		cmp eax, 0x0F
		jne not_0x0F
		mov eax, 0x10
		not_0x0F:
		movzx edx, byte ptr[eax + 0x0045887C]
		jmp dword ptr[edx * 4 + 0x0045885C]
	}
}

byte originalExplorationCrimeInstructions[6];
void enableExploreAnywhere() {
	byte nops[6] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	overwriteInstructions((void*)EXPLORATION_CRIME_INSTRUCTION_ADDRESS, nops, sizeof(nops), originalExplorationCrimeInstructions);
}

void disableExploreAnywhere() {
	restoreInstructions((void*)EXPLORATION_CRIME_INSTRUCTION_ADDRESS, originalExplorationCrimeInstructions, sizeof(originalExplorationCrimeInstructions));
}