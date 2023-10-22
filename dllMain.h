#pragma once
//#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <d3d9.h>
#include <Windows.h>
#include <Xinput.h>

// derived from 5-byte JMP far opcode, -1 because of push esi created by Visual Studio
#define FAR_JUMP_OFFSET (-4)

#define GAME_LOOP_FUNCTION_CALL				0x0053C096
#define GAME_LOOP_FUNCTION_ADDRESS			0x005684A0

#define KEY_PRESSED_MASK 0x1
#define KEY_DOWN_MASK 0x8000

#define distance(a, b, c) ((float)sqrt(a*a + b*b + c*c))
#define distance2d(a, b) ((float)sqrt(a*a + b*b))

BOOL WINAPI DllMain(__in  HINSTANCE hinstDLL, __in  DWORD fdwReason, __in  LPVOID lpvReserved);
void APIENTRY drawScene(LPDIRECT3DDEVICE9 pDevice);
void init();
void detour();
void hack();
void exit();
void unload();

extern WantedLevel wantedLevel;
extern Pedestrian player;
extern Vehicle vehicle;

extern bool usingGamepad;
extern XINPUT_STATE gamepadState;
extern WORD buttonsHeld;
extern WORD buttonsPressed;
extern WORD buttonsReleased;
extern WORD previousButtonState;

extern DWORD vehicleBaseAddressLastFrame;