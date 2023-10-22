#include <fstream>

#include "D3D9EndSceneHook.h"

DWORD* LPDIRECT3DDEVICE9vTable = NULL;
DWORD endSceneAddress = NULL;
EndSceneFunction endScene;

DrawingFunction userDrawingFunction;

void continueDetourInstallSeparateThread(char* windowName) {
	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d) {
		return;
	}
	IDirect3DDevice9* dummyDevice = nullptr;
	D3DPRESENT_PARAMETERS params = {};
	params.Windowed = false;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	HWND handle = FindWindow(NULL, windowName);
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
	LPDIRECT3DDEVICE9vTable = *vtable;
	endSceneAddress = LPDIRECT3DDEVICE9vTable[END_SCENE_VTABLE_INDEX];
	endScene = (EndSceneFunction)endSceneAddress;
#ifdef D3D9_HOOK_DEBUG
	ofstream f;
	f.open("endscene.txt");
	f << hex << endSceneAddress << endl << (*vtable + 42);
	f.close();
#endif
	(*vtable)[END_SCENE_VTABLE_INDEX] = (DWORD)&endSceneDetour;
	dummyDevice->Release();
}

void installD3D9Hook(char* windowName, DrawingFunction drawingFunction) {
	userDrawingFunction = drawingFunction;
	// Install the Direct3D9 hook via a new thread (separate thread required to prevent deadlock)
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)continueDetourInstallSeparateThread, (void*)windowName, 0, 0);
}

void uninstallD3D9Hook() {
	LPDIRECT3DDEVICE9vTable[END_SCENE_VTABLE_INDEX] = endSceneAddress;
}

HRESULT WINAPI endSceneDetour(LPDIRECT3DDEVICE9 pDevice) {
	userDrawingFunction(pDevice);
	return endScene(pDevice);
}