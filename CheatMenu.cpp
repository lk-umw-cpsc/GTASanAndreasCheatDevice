#include "CheatMenu.h"

#include <string>

using namespace std;

void doNothing() {
	
}

string CheatMenuItem::getText() {
	return "";
}

// void MenuItem::onLeftInput() {

// }

// void MenuItem::onRightInput() {

// }

// void MenuItem::onActivate() {

// }

CheatMenu::CheatMenu(CheatMenuItem** menuItems, int numMenuItems) {
	this->menuItems = menuItems;
	this->numMenuItems = numMenuItems;
	selectedMenuItem = menuItems[0];
	selectedMenuItemIndex = 0;
}

void CheatMenu::menuUp() {
	selectedMenuItemIndex--;
	if (selectedMenuItemIndex == -1) {
		selectedMenuItemIndex = numMenuItems - 1;
	}
	selectedMenuItem = menuItems[selectedMenuItemIndex];
}

void CheatMenu::menuDown() {
	selectedMenuItemIndex++;
	if (selectedMenuItemIndex == numMenuItems) {
		selectedMenuItemIndex = 0;
	}
	selectedMenuItem = menuItems[selectedMenuItemIndex];
}

void CheatMenu::selectedMenuItemLeft() {
	selectedMenuItem->onLeftInput();
}

void CheatMenu::selectedMenuItemRight() {
	selectedMenuItem->onRightInput();
}

void CheatMenu::activateSelectedMenuItem() {
	selectedMenuItem->onActivate();
}

ActiveCheatMenuItem::ActiveCheatMenuItem(string name, voidFunction onEnable, voidFunction onDisable, voidFunction onFrame) {
	this->onEnable = onEnable;
	this->onDisable = onDisable;
	this->onFrame = onFrame;
	this->name = name;
}

void ActiveCheatMenuItem::toggle() {
	enabled = !enabled;
	if (enabled && onEnable != nullptr) {
		onEnable();
	} else if(!enabled && onDisable != nullptr) {
		onDisable();
	}
}

void ActiveCheatMenuItem::setEnabled(bool enabled) {
	bool oldState = this->enabled;
	this->enabled = enabled;
	if (oldState != enabled) {
		if (enabled && onEnable) {
			onEnable();
		} else if (!enabled && onDisable) {
			onDisable();
		}
	}
}

void ActiveCheatMenuItem::onLeftInput() {
	toggle();
}

void ActiveCheatMenuItem::onRightInput() {
	toggle();
}

void ActiveCheatMenuItem::onLeftActivate() {
	toggle();
}