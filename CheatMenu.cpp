#include "CheatMenu.h"

#include <string>

using namespace std;

string CheatMenuItem::getText() {
	return "";
}

void CheatMenuItem::onLeftInput() {

}

void CheatMenuItem::onRightInput() {

}

void CheatMenuItem::onActivate() {

}

bool CheatMenuItem::canBeActivated() {
	return true;
}

CheatMenu::CheatMenu(CheatMenuItem** menuItems, int numMenuItems, int horizontalAnchor, int verticalAnchor, int padding, int horizontalMargin, int verticalMargin) {
	this->menuItems = menuItems;
	this->numMenuItems = numMenuItems;
	selectedMenuItem = menuItems[0];
	selectedMenuItemIndex = 0;
	this->horizontalAnchor = horizontalAnchor;
	this->verticalAnchor = verticalAnchor;
	this->padding = padding;
	this->horizontalMargin = horizontalMargin;
	this->verticalMargin = verticalMargin;
}

void CheatMenu::menuUp() {
	int oldIndex = selectedMenuItemIndex;
	do {
		selectedMenuItemIndex--;
		if (selectedMenuItemIndex == -1) {
			selectedMenuItemIndex = numMenuItems - 1;
		}
		selectedMenuItem = menuItems[selectedMenuItemIndex];
	} while (!selectedMenuItem->canBeActivated() && selectedMenuItemIndex != oldIndex);
}

void CheatMenu::menuDown() {
	int oldIndex = selectedMenuItemIndex;
	do {
		selectedMenuItemIndex++;
		if (selectedMenuItemIndex == numMenuItems) {
			selectedMenuItemIndex = 0;
		}
		selectedMenuItem = menuItems[selectedMenuItemIndex];
	} while (!selectedMenuItem->canBeActivated() && selectedMenuItemIndex != oldIndex);
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

CheatMenuItem* CheatMenu::getMenuItem(int index) {
	return menuItems[index];
}

ActiveCheatMenuItem::ActiveCheatMenuItem(voidFunction onEnable, voidFunction onDisable, voidFunction onFrame, string name, bool enabled) {
	this->onEnable = onEnable;
	this->onDisable = onDisable;
	this->onFrame = onFrame;
	this->name = name;
	this->enabled = enabled;
	if (enabled && onEnable) {
		onEnable();
	}
}

void ActiveCheatMenuItem::toggle() {
	enabled = !enabled;
	if (enabled && onEnable != nullptr) {
		onEnable();
	} else if(!enabled && onDisable != nullptr) {
		onDisable();
	}
}

void ActiveCheatMenuItem::setEnabled(bool newState) {
	bool oldState = enabled;
	enabled = newState;
	if (oldState != newState) {
		if (newState && onEnable) {
			onEnable();
		} else if (!newState && onDisable) {
			onDisable();
		}
	}
}

bool ActiveCheatMenuItem::isEnabled() {
	return enabled;
}

void ActiveCheatMenuItem::onLeftInput() {
	toggle();
}

void ActiveCheatMenuItem::onRightInput() {
	toggle();
}

void ActiveCheatMenuItem::onActivate() {
	toggle();
}

string ActiveCheatMenuItem::getText() {
	string stateText;
	if (enabled) {
		stateText = " ON";
	}
	else {
		stateText = " OFF";
	}
	return name + stateText;
}