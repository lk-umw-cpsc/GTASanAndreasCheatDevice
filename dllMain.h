#pragma once
#include <Windows.h>

BOOL WINAPI DllMain(__in  HINSTANCE hinstDLL, __in  DWORD fdwReason, __in  LPVOID lpvReserved);
void init();
void detour();
void hack();
void exit();
void noWantedLevel();
void infiniteHealth();
void infiniteHealthOff();
void infiniteCarHealth();
void hoverCar();
void autoFlipCar();
void installFallOffBikeDetour();
void fallOffBikeDetour();
void uninstallFallOffBikeDetour();
void enableInfiniteAmmo();
void disableInfiniteAmmo();