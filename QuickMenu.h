#pragma once

#include <string>

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
	virtual void show() = 0;
	void menuUp();
	void menuDown();
	void selectedMenuItemLeft();
	void selectedMenuItemRight();
	void activateSelectedMenuItem();
};