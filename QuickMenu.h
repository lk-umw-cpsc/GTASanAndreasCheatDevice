#pragma once
#include <string>

#include <d3dx9core.h>

using namespace std;

class QuickMenuItem {
public:
	virtual string getText();
	virtual void onLeftInput();
	virtual void onRightInput();
	virtual void onActivate();
};

class QuickMenu {
protected:
	QuickMenuItem** menuItems;
	QuickMenuItem* selectedMenuItem;
	int numMenuItems;
	int selectedMenuItemIndex;
public:
	QuickMenu(QuickMenuItem** menuItems, int numMenuItems);
	virtual void show(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT pFont, LPD3DXSPRITE pSprite) = 0;
	void menuUp();
	void menuDown();
	void selectedMenuItemLeft();
	void selectedMenuItemRight();
	void activateSelectedMenuItem();
};