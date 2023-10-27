#pragma once
#include <string>
#include <d3dx9core.h>

using namespace std;

#define HORIZONTAL_ANCHOR_LEFT		0
#define HORIZONTAL_ANCHOR_CENTER	1
#define HORIZONTAL_ANCHOR_RIGHT		2

#define VERTICAL_ANCHOR_TOP			0
#define VERTICAL_ANCHOR_CENTER		1
#define VERTICAL_ANCHOR_BOTTOM		2

typedef void(*voidFunction)();

typedef struct MenuStyle {
	int horizontalAlignment;
	int verticalAlignment;
	float horizontalMargin;
	float verticalMargin;
	float horizontalPadding;
	float verticalPadding;
	float fontSize;
	float lineSpacing;
	LPD3DXFONT pFont;
	LPD3DXSPRITE pSprite;
};

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

class CheatMenu {
protected:
	CheatMenuItem** menuItems;
	CheatMenuItem* selectedMenuItem;
	MenuStyle* style;
	int numMenuItems;
	int selectedMenuItemIndex;
public:
	CheatMenu(CheatMenuItem** menuItems, int numMenuItems, MenuStyle* style);
	virtual void show(LPDIRECT3DDEVICE9 pDevice) = 0;
	void menuUp();
	void menuDown();
	void selectedMenuItemLeft();
	void selectedMenuItemRight();
	void activateSelectedMenuItem();
	void releaseD3DObjects();
	CheatMenuItem* getMenuItem(int index);
};