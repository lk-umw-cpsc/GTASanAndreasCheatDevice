#pragma once
#include <string>

#include <d3dx9core.h>

using namespace std;

void doNothing();

typedef void(*voidFunction)();

class CheatMenuItem {
public:
	virtual string getText();
	virtual void onLeftInput() = doNothing;
	virtual void onRightInput() = doNothing;
	virtual void onActivate() = doNothing;
};

class ActiveCheatMenuItem : public CheatMenuItem {
protected:
	string name;
	bool enabled;
public:
	voidFunction onEnable;
	voidFunction onDisable;
	voidFunction onFrame;
	ActiveCheatMenuItem(string name, void (*onEnable)(), void (*onDisable)(), void (*onFrame)());
	void toggle();
	void setEnabled(bool enabled);
}

class CheatMenu {
protected:
	CheatMenu** menuItems;
	CheatMenu* selectedMenuItem;
	int numMenuItems;
	int selectedMenuItemIndex;
public:
	CheatMenu(CheatMenuItem** menuItems, int numMenuItems);
	virtual void show(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT pFont, LPD3DXSPRITE pSprite) = 0;
	void menuUp();
	void menuDown();
	void selectedMenuItemLeft();
	void selectedMenuItemRight();
	void activateSelectedMenuItem();
};