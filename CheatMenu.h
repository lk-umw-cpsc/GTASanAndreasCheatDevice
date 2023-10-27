#pragma once
#include <string>
#include <d3dx9core.h>

using namespace std;

typedef void(*voidFunction)();

class CheatMenuItem {
public:
	virtual string getText();
	virtual void onLeftInput();
	virtual void onRightInput();
	virtual void onActivate();
	virtual bool canBeActivated();
};

class ActiveCheatMenuItem : public CheatMenuItem {
protected:
	string name;
	bool enabled;
public:
	voidFunction onEnable;
	voidFunction onDisable;
	voidFunction onFrame;
	ActiveCheatMenuItem(voidFunction onEnable, voidFunction onDisable, voidFunction onFrame, string name, bool enabled);
	void toggle();
	void setEnabled(bool enabled);
	bool isEnabled();
	string getText();
	void onLeftInput();
	void onRightInput();
	void onActivate();
};

#define HORIZONTAL_ANCHOR_LEFT		0
#define HORIZONTAL_ANCHOR_CENTER	1
#define HORIZONTAL_ANCHOR_RIGHT		2

#define VERTICAL_ANCHOR_TOP			0
#define VERTICAL_ANCHOR_CENTER		1
#define VERTICAL_ANCHOR_BOTTOM		2

class CheatMenu {
protected:
	CheatMenuItem** menuItems;
	CheatMenuItem* selectedMenuItem;
	int numMenuItems;
	int selectedMenuItemIndex;
	int horizontalAnchor, verticalAnchor;
	int padding;
	int horizontalMargin;
	int verticalMargin;
public:
	CheatMenu(CheatMenuItem** menuItems, int numMenuItems, int horizontalAnchor, int verticalAnchor, int padding, int horizontalMargin, int verticalMargin);
	virtual void show(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT pFont, LPD3DXSPRITE pSprite) = 0;
	void menuUp();
	void menuDown();
	void selectedMenuItemLeft();
	void selectedMenuItemRight();
	void activateSelectedMenuItem();
	CheatMenuItem* getMenuItem(int index);
};