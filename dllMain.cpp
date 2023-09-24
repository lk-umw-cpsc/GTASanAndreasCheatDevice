#include <Windows.h>
#include <d3d9.h>
#include <string>
#include <math.h>
#include <Xinput.h>

#include "GTA.h"
#include "Cheat.h"
#include "dllMain.h"

/*
	Ideas:
	Player movement speed
	Gravity
	Player jump size (Moon jump)
	Punt
	Vehicle mass
	Car speed
	Unlock cars
	Spawn cars
	Crimes
	Menu
*/

#define GAME_LOOP_FUNCTION_CALL		0x0053C096
#define GAME_LOOP_FUNCTION_ADDRESS	0x005684A0

#define DRAW_STRING_FUNCTION_ADDRESS (EXE_OFFSET + 0x29DB70)

#define KEY_PRESSED_MASK 0x1
#define KEY_DOWN_MASK 0x8000

#define distance(a, b, c) (float)sqrt(a*a + b*b + c*c)
#define distance2d(a, b) (float)sqrt(a*a + b*b)

const void(*gameLoop)() = (const void(*)()) GAME_LOOP_FUNCTION_ADDRESS;

HWND hwnd;

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

// Non-working D3D hook
// void* d3d9DeviceVTable[119];
//void fetchD3D9VTable() {
//	IDirect3D9* d3dSys = Direct3DCreate9(D3D_SDK_VERSION);
//	IDirect3DDevice9* dummyDevice = nullptr;
//
//	D3DPRESENT_PARAMETERS params = {};
//	params.Windowed = true;
//	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
//	params.hDeviceWindow = hwnd;
//
//	//HRESULT dummyDevice = 
//	d3dSys->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, params.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &dummyDevice);
//	
//	//memcpy(d3d9DeviceVTable, *reinterpret_cast<void***>(dummyDevice), sizeof(d3d9DeviceVTable));
//	dummyDevice->Release();
//	d3dSys->Release();
//}
//
//const char* REL_JMP = "\xE9";
//const char* NOP = "\x90";
//// 1 byte instruction +  4 bytes address
//const unsigned int SIZE_OF_REL_JMP = 5;
//
//// adapted from https://guidedhacking.com/threads/simple-x86-c-trampoline-hook.14188/
//// hookedFn: The function that's about to the hooked
//// hookFn: The function that will be executed before `hookedFn` by causing `hookFn` to take a detour
//void* WINAPI hookFn(char* hookedFn, char* hookFn, int copyBytesSize, unsigned char* backupBytes, std::string descr)
//{
//
//	if (copyBytesSize < 5)
//	{
//		// the function prologue of the hooked function
//		// should be of size 5 (or larger)
//		return nullptr;
//	}
//
//	//
//	// 1. Backup the original function prologue
//	//
//	if (!ReadProcessMemory(GetCurrentProcess(), hookedFn, backupBytes, copyBytesSize, 0))
//	{
//		MessageBox(0, std::string("[hookFn] Failed to Backup Original Bytes for " + descr).c_str(), ":(", 0);
//		return nullptr;
//	}
//
//	//
//	// 2. Setup the trampoline
//	// --> Cause `hookedFn` to return to `hookFn` without causing an infinite loop
//	// Otherwise calling `hookedFn` directly again would then call `hookFn` again, and so on :)
//	//
//	// allocate executable memory for the trampoline
//	// the size is (amount of bytes copied from the original function) + (size of a relative jump + address)
//
//	char* trampoline = (char*)VirtualAlloc(0, copyBytesSize + SIZE_OF_REL_JMP, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
//
//	// steal the first `copyBytesSize` bytes from the original function
//	// these will be used to make the trampoline work
//	// --> jump back to `hookedFn` without executing `hookFn` again
//	memcpy(trampoline, hookedFn, copyBytesSize);
//	// append the relative JMP instruction after the stolen instructions
//	memcpy(trampoline + copyBytesSize, REL_JMP, sizeof(REL_JMP));
//
//	// calculate the offset between the hooked function and the trampoline
//	// --> distance between the trampoline and the original function `hookedFn`
//	// this will land directly *after* the inserted JMP instruction, hence subtracting 5
//	int hookedFnTrampolineOffset = hookedFn - trampoline - SIZE_OF_REL_JMP;
//	memcpy(trampoline + copyBytesSize + 1, &hookedFnTrampolineOffset, sizeof(hookedFnTrampolineOffset));
//
//	//
//	// 3. Detour the original function `hookedFn`
//	// --> cause `hookedFn` to execute `hookFn` first
//	// remap the first few bytes of the original function as RXW
//	DWORD oldProtect;
//	if (!VirtualProtect(hookedFn, copyBytesSize, PAGE_EXECUTE_READWRITE, &oldProtect))
//	{
//		MessageBox(0, std::string("[hookFn] Failed to set RXW for " + descr).c_str(), ":(", 0);
//		return nullptr;
//	}
//
//	// best variable name ever
//	// calculate the size of the relative jump between the start of `hookedFn` and the start of `hookFn`.
//	int hookedFnHookFnOffset = hookFn - hookedFn - SIZE_OF_REL_JMP;
//
//	// Take a relative jump to `hookFn` at the beginning
//	// of course, `hookFn` has to expect the same parameter types and values
//	memcpy(hookedFn, REL_JMP, sizeof(REL_JMP));
//	memcpy(hookedFn + 1, &hookedFnHookFnOffset, sizeof(hookedFnHookFnOffset));
//
//	// restore the previous protection values
//	if (!VirtualProtect(hookedFn, copyBytesSize, oldProtect, &oldProtect))
//	{
//		MessageBox(0, std::string("[hookFn] Failed to Restore Protection for " + descr).c_str(), ":(", 0);
//	}
//
//	return trampoline;
//}

WantedLevel wantedLevel;
Pedestrian player;
Vehicle vehicle;

Cheat* cheats[] = {
	// onEnable, onDisable, onFrame
	new Cheat(nullptr, nullptr, &noWantedLevel),
	new Cheat(nullptr, &infiniteHealthOff, &infiniteHealth),
	new Cheat(nullptr, nullptr, &infiniteCarHealth),
	new Cheat(nullptr, nullptr, &autoFlipCar),
	new Cheat(nullptr, nullptr, &hoverCar),
	new Cheat(&installFallOffBikeDetour, &uninstallFallOffBikeDetour, nullptr),
	new Cheat(&enableInfiniteAmmo, &disableInfiniteAmmo, nullptr),
};
const int numCheats = sizeof(cheats) / sizeof(Cheat*);

void (*endSceneTrampoline)(LPDIRECT3DDEVICE9);
void endSceneHook(LPDIRECT3DDEVICE9 pDevice) {
	D3DRECT r = { 10, 10, 200, 200 };
	pDevice->Clear(1, &r, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 0), 0, 0);
	endSceneTrampoline(pDevice);
}

unsigned int originalFunctionCall;
void init() {
	//hwnd = FindWindow(NULL, "GTA: San Andreas");

	//fetchD3D9VTable();
	//void* endSceneAddress = d3d9DeviceVTable[42];
	//unsigned char endSceneBytes[7];
	//endSceneTrampoline = (void(*)(LPDIRECT3DDEVICE9))hookFn((char*)endSceneAddress, (char*)endSceneHook, 7, endSceneBytes, (char*)"");
	hookWantedLevel(*pWantedLevelBaseAddress, &wantedLevel);
	hookPedestrian(*pPlayerPedBaseAddress, &player);
	cheats[0]->setEnabled(true);
	cheats[1]->setEnabled(true);
	cheats[2]->setEnabled(true);
	cheats[3]->setEnabled(true);
	cheats[5]->setEnabled(true);
	cheats[6]->setEnabled(true);

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

bool usingGamepad;
XINPUT_STATE gamepadState;
WORD buttonsHeld;
WORD buttonsPressed;
WORD buttonsReleased;
WORD previousButtonState;
void hack() {
	unsigned int vehicleBaseAddress = *pPlayerVehicleBaseAddress;
	if (vehicleBaseAddress) {
		hookVehicle(vehicleBaseAddress, &vehicle);
	} else {
		vehicle.baseAddress = NULL;
	}
	if (GetAsyncKeyState(VK_NUMPAD3) & KEY_PRESSED_MASK) {
		cheats[4]->toggle();
	}
	if (GetAsyncKeyState(VK_NUMPAD4) & KEY_PRESSED_MASK) {
		cheats[5]->toggle();
	}
	if (GetAsyncKeyState(VK_NUMPAD5) & KEY_PRESSED_MASK) {
		cheats[6]->toggle();
	}
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

#define HOVER_CAR_MAX_VELOCITY 3.0f
#define HOVER_CAR_ACCELERATION 0.1f
#define HOVER_CAR_FRICTION 0.1f
#define HOVER_CAR_FORWARD_FORCE (1.0f - HOVER_CAR_FRICTION)
void hoverCar() {
	if (!vehicle.baseAddress) {
		return;
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
	DWORD old;
	newJumpAddress = (DWORD)&fallOffBikeDetour - 0x4b4ac0 - 0x05;
	BYTE detour[] = { 0xE9, 0, 0, 0, 0 }; //E9 for jmp
	memcpy((LPVOID)(detour + 1), (LPVOID)&newJumpAddress, sizeof(newJumpAddress));
	VirtualProtect((LPVOID)0x4b4ac0, sizeof(detour), PAGE_EXECUTE_READWRITE, &old);
	memcpy((LPVOID)&originalFallOffBikeCall, (LPVOID)0x4b4ac0, sizeof(originalFallOffBikeCall));
	memcpy((LPVOID)0x4b4ac0, (LPVOID)detour, sizeof(detour));
	VirtualProtect((LPVOID)0x4b4ac0, sizeof(detour), old, &old);
}

void fallOffBikeDetour() {
	//unsigned int returnAddress = 0x4b4ac5;
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
		push 0x4b4ac5 // original return address
		ret
	}
}

void uninstallFallOffBikeDetour() {
	DWORD pHook;
	DWORD old;
	VirtualProtect((LPVOID)0x4b4ac0, sizeof(originalFallOffBikeCall), PAGE_EXECUTE_READWRITE, &old);
	memcpy((LPVOID)0x4b4ac0, (LPVOID)&originalFallOffBikeCall, sizeof(originalFallOffBikeCall));
	VirtualProtect((LPVOID)0x4b4ac0, sizeof(originalFallOffBikeCall), old, &old);
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
	DWORD old;
	BYTE nops[WEAPON_AMMO_DECREMENT_SIZE] = { 0x90, 0x90, 0x90 };
	for (int i = 0; i < NUM_AMMO_DECREMENT_OP_CODES; i++) {
		VirtualProtect((LPVOID)(infiniteAmmoNOPTargets[i]), WEAPON_AMMO_DECREMENT_SIZE, PAGE_EXECUTE_READWRITE, &old);
		memcpy((LPVOID)(&originalAmmoDecrementCode[i][0]), (LPVOID)(infiniteAmmoNOPTargets[i]), WEAPON_AMMO_DECREMENT_SIZE);
		memcpy((LPVOID)(infiniteAmmoNOPTargets[i]), (LPVOID)nops, WEAPON_AMMO_DECREMENT_SIZE);
		VirtualProtect((LPVOID)(infiniteAmmoNOPTargets[i]), WEAPON_AMMO_DECREMENT_SIZE, old, &old);
	}
}

void disableInfiniteAmmo() {
	DWORD old;
	for (int i = 0; i < NUM_AMMO_DECREMENT_OP_CODES; i++) {
		VirtualProtect((LPVOID)(infiniteAmmoNOPTargets[i]), WEAPON_AMMO_DECREMENT_SIZE, PAGE_EXECUTE_READWRITE, &old);
		memcpy((LPVOID)(infiniteAmmoNOPTargets[i]), (LPVOID)(&originalAmmoDecrementCode[i][0]), WEAPON_AMMO_DECREMENT_SIZE);
		VirtualProtect((LPVOID)infiniteAmmoNOPTargets[i], WEAPON_AMMO_DECREMENT_SIZE, old, &old);
	}
}