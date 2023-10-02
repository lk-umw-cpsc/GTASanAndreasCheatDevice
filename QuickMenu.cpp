#include "QuickMenu.h"

#include <string>

using namespace std;

string QuickMenuItem::getText() {
	return "";
}

void QuickMenuItem::onLeftInput() {

}

void QuickMenuItem::onRightInput() {

}

void QuickMenuItem::onActivate() {

}

QuickMenu::QuickMenu(QuickMenuItem** menuItems, int numMenuItems) {
	this->menuItems = menuItems;
	this->numMenuItems = numMenuItems;
	selectedMenuItem = menuItems[0];
	selectedMenuItemIndex = 0;
}

void QuickMenu::menuUp() {
	selectedMenuItemIndex--;
	if (selectedMenuItemIndex == -1) {
		selectedMenuItemIndex = numMenuItems - 1;
	}
	selectedMenuItem = menuItems[selectedMenuItemIndex];
}

void QuickMenu::menuDown() {
	selectedMenuItemIndex++;
	if (selectedMenuItemIndex == numMenuItems) {
		selectedMenuItemIndex = 0;
	}
	selectedMenuItem = menuItems[selectedMenuItemIndex];
}

void QuickMenu::selectedMenuItemLeft() {
	selectedMenuItem->onLeftInput();
}

void QuickMenu::selectedMenuItemRight() {
	selectedMenuItem->onRightInput();
}

void QuickMenu::activateSelectedMenuItem() {
	selectedMenuItem->onActivate();
}