#pragma once
#include <string>
#include <d3dx9core.h>
#include <Xinput.h>

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

#define INPUT_TYPE_BUTTON_HELD 0
#define INPUT_TYPE_BUTTON_PRESSED 1
#define INPUT_TYPE_BUTTON_RELEASED 2

typedef struct ButtonInput {
	ButtonInput() : inputType(0), button(0) {

	}
	ButtonInput(BYTE a, WORD b) : inputType(a), button(b) {

	}
	BYTE inputType;
	WORD button;
};

typedef struct MenuHotKey {
	const ButtonInput* inputs;
	const int numInputs;
};

bool inputActive(ButtonInput input, DWORD buttonStates[3]);
bool hotKeyActivated(const MenuHotKey* hk, DWORD buttonStates[3]);

MenuHotKey** createHotKeys(string s);

typedef struct CheatMenuHotKeys {
	const MenuHotKey* menuUp;
	const MenuHotKey* menuDown;
	const MenuHotKey* menuLeft;
	const MenuHotKey* menuRight;
	const MenuHotKey* menuActivate;
	const MenuHotKey* menuClose;
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
	CheatMenuHotKeys* hotKeys;
	int numMenuItems;
	int selectedMenuItemIndex;
	bool showing;
public:
	CheatMenu(CheatMenuItem** menuItems, int numMenuItems, MenuStyle* style, CheatMenuHotKeys* hotKeys);
	virtual void show(LPDIRECT3DDEVICE9 pDevice) = 0;
	void menuUp();
	void menuDown();
	void selectedMenuItemLeft();
	void selectedMenuItemRight();
	void activateSelectedMenuItem();
	void releaseD3DObjects();
	void handleInput(DWORD buttonsHeld, DWORD buttonsPressed, DWORD buttonsReleased);
	CheatMenuItem* getMenuItem(int index);
};