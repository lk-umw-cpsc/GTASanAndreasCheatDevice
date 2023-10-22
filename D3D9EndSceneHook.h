#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9core.h>

#define END_SCENE_VTABLE_INDEX 42

typedef HRESULT(WINAPI* EndSceneFunction)(LPDIRECT3DDEVICE9);
typedef void (WINAPI *DrawingFunction)(LPDIRECT3DDEVICE9 pDevice);

void installD3D9Hook(char* windowName, DrawingFunction drawingFunction);
void uninstallD3D9Hook();
HRESULT WINAPI endSceneDetour(LPDIRECT3DDEVICE9 pDevice);