#pragma once
#include <d3dx9core.h>

#include "CheatMenu.h"
#include "dllMain.h"

#define MENU_NEW_LINE "~N~"
#define MENU_WHITE_TEXT_TOKEN "~w~"
#define MENU_PURPLE_TEXT_TOKEN "~p~"

#define MENU_TEXT_COLOR								D3DCOLOR_ARGB(255, 255, 255, 255)
#define MENU_DEACTIVATED_TEXT_COLOR					D3DCOLOR_XRGB(140, 140, 140)
#define MENU_SELECTED_TEXT_COLOR					D3DCOLOR_ARGB(255, 189, 136, 215)
#define MENU_SELECTED_DEACTIVATED_TEXT_COLOR		D3DCOLOR_XRGB(183, 156, 196)
#define MENU_BACKGROUND_COLOR						D3DCOLOR_XRGB(32, 32, 32)

class GTASACheatMenu : public CheatMenu {
public:
	GTASACheatMenu(CheatMenuItem** menuItems, int numMenuItems, MenuStyle* style, CheatMenuControls* hotKeys);
	void show(LPDIRECT3DDEVICE9 pDevice);
};

class SpawnCarMenuItem : public CheatMenuItem {
private:
	int selectedCarID;
public:
	SpawnCarMenuItem(int defaultCarID);
	string getText();
	void onLeftInput();
	void onRightInput();
	void onActivate();
	string getSelectedVehicleName();
};

class RepairVehicleMenuItem : public CheatMenuItem {
public:
	string getText();
	void onActivate();
	bool canBeActivated();
};

class SelfDestructMenuItem : public CheatMenuItem {
public:
	string getText();
	void onActivate();
	bool canBeActivated();
};

class StepForwardMenuItem : public CheatMenuItem {
public:
	string getText();
	void onActivate();
};

class StepUpMenuItem : public CheatMenuItem {
public:
	string getText();
	void onActivate();
};

class StepDownMenuItem : public CheatMenuItem {
public:
	string getText();
	void onActivate();
};

class QuickTakeOffMenuItem : public CheatMenuItem {
public:
	string getText();
	void onActivate();
};

class VehicleColorMenuItem : public CheatMenuItem {
public:
	string getText();
	void onLeftInput();
	void onRightInput();
	int getCurrentVehicleColor();
	bool canBeActivated();
};

class KillEveryoneMenuItem : public CheatMenuItem {
public:
	string getText();
	void onActivate();
};

class BlowUpAllVehicles : public CheatMenuItem {
public:
	string getText();
	void onActivate();
};

class ChangeGravityMenuItem : public CheatMenuItem {
private:
	float defaultGravity;
public:
	ChangeGravityMenuItem();
	string getText();
	void onLeftInput();
	void onRightInput();
	void onActivate();
};

class BunnyHopMenuItem : public CheatMenuItem {
private:
	float defaultMultiplier;
public:
	BunnyHopMenuItem();
	string getText();
	void onLeftInput();
	void onRightInput();
	void onActivate();
};

void initDebugMenu(LPDIRECT3DDEVICE9 pDevice, D3DVIEWPORT9* viewport);
void drawDebugMenu(LPDIRECT3DDEVICE9 pDevice);
void drawDebugLine(LPDIRECT3DDEVICE9 pDevice, string s, int line);