#pragma once
#include <d3dx9core.h>

#include "QuickMenu.h"
#include "dllMain.h"

#define MENU_NEW_LINE "~N~"
#define MENU_WHITE_TEXT_TOKEN "~w~"
#define MENU_PURPLE_TEXT_TOKEN "~p~"

#define MENU_TEXT_COLOR				D3DCOLOR_ARGB(255, 255, 255, 255)
#define MENU_SELECTED_TEXT_COLOR	D3DCOLOR_ARGB(255, 203, 99, 255)
#define MENU_BACKGROUND_COLOR		D3DCOLOR_XRGB(32, 32, 32)

class GTASAQuickMenu : public QuickMenu {
public:
	GTASAQuickMenu(QuickMenuItem** menuItems, int numMenuItems);
	void show(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT pFont, LPD3DXSPRITE pSprite);
};

class SpawnCarMenuItem : public QuickMenuItem {
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

class RepairVehicleMenuItem : public QuickMenuItem {
public:
	string getText();
	void onActivate();
};

class SelfDestructMenuItem : public QuickMenuItem {
public:
	string getText();
	void onActivate();
};

class StepForwardMenuItem : public QuickMenuItem {
public:
	string getText();
	void onActivate();
};

class StepUpMenuItem : public QuickMenuItem {
public:
	string getText();
	void onActivate();
};

class StepDownMenuItem : public QuickMenuItem {
public:
	string getText();
	void onActivate();
};

class QuickTakeOffMenuItem : public QuickMenuItem {
public:
	string getText();
	void onActivate();
};