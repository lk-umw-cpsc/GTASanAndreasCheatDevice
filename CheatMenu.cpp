#include "CheatMenu.h"

#include <string>
#include <vector>

using namespace std;

#define TOKEN_HOLD 0
#define TOKEN_PRESS 1
#define TOKEN_RELEASE 2

#define TOKEN_UP 3
#define TOKEN_DOWN 4
#define TOKEN_LEFT 5
#define TOKEN_RIGHT 6
#define TOKEN_A 7
#define TOKEN_B 8
#define TOKEN_X 9
#define TOKEN_Y 10
#define TOKEN_LB 11
#define TOKEN_RB 12

#define TOKEN_AND 13
#define TOKEN_OR 14

const char* tokenStrings[] = {
	"hold", "press", "release", "up", "down", "left", "right", "a", "b", "x", "y", "lb", "rb", "and", "or"
};
#define NUM_TOKENS (sizeof(tokenStrings) / sizeof(char*))

MenuHotKey** createHotKeys(char* keyString) {
	const int len = strlen(keyString);
	if (len > 256) {
		return NULL;
	}
	char* copy = new char[256];
	strcpy_s(copy, 256, keyString);
	// hold LB and press up
	int count = 0, i = 0;
	vector<char*> needles;
	bool lastWasSpace = true;
	for (char *pCurrentChar = copy, currentChar = *copy; currentChar; pCurrentChar++) {
		if (currentChar != ' ') {
			if (lastWasSpace) {
				lastWasSpace = false;
				count++;
				needles.push_back(pCurrentChar);
			}
		}
		else {
			*pCurrentChar = 0;
			lastWasSpace = true;
		}
		i++;
	}
	const int numSymbols = needles.size();
	int* symbols = new int[numSymbols];
	for (i = 0; i < numSymbols; i++) {
		char* curToken = needles.at(i);
		bool found = false;
		for (int t = 0; t < NUM_TOKENS; t++) {
			if (_strcmpi(curToken, tokenStrings[t]) == 0) {
				symbols[i] = t;
				found = true;
				break;
			}
		}
		if (!found) {
			return NULL;
		}
	}
	BYTE buttonState;
	WORD button;
	vector<ButtonInput> inputs;
	vector<MenuHotKey*> hotkeys;
	int state = 0;
	for (i = 0; i < numSymbols; i++) {
		int currentSymbol = symbols[i];
		if (state == 0) {
			switch (currentSymbol) {
			case TOKEN_HOLD:
				buttonState = INPUT_TYPE_BUTTON_HELD;
				break;
			case TOKEN_PRESS:
				buttonState = INPUT_TYPE_BUTTON_PRESSED;
				break;
			case TOKEN_RELEASE:
				buttonState = INPUT_TYPE_BUTTON_RELEASED;
				break;
			default:
				return NULL;
			}
			state = 1;
		}
		else if (state == 1) {
			switch (currentSymbol) {
			case TOKEN_UP:
				button = XINPUT_GAMEPAD_DPAD_UP;
				break;
			case TOKEN_DOWN:
				button = XINPUT_GAMEPAD_DPAD_DOWN;
				break;
			case TOKEN_LEFT:
				button = XINPUT_GAMEPAD_DPAD_LEFT;
				break;
			case TOKEN_RIGHT:
				button = XINPUT_GAMEPAD_DPAD_RIGHT;
				break;
			case TOKEN_A:
				button = XINPUT_GAMEPAD_A;
				break;
			case TOKEN_B:
				button = XINPUT_GAMEPAD_B;
				break;
			case TOKEN_X:
				button = XINPUT_GAMEPAD_X;
				break;
			case TOKEN_Y:
				button = XINPUT_GAMEPAD_Y;
				break;
			case TOKEN_LB:
				button = XINPUT_GAMEPAD_LEFT_SHOULDER;
				break;
			case TOKEN_RB:
				button = XINPUT_GAMEPAD_RIGHT_SHOULDER;
				break;
			default:
				return NULL;
			}
			ButtonInput input = { buttonState, button };
			inputs.push_back(input);
			state = 2;
		}
		else if (state == 2) {
			ButtonInput* hotKeyInputs;
			MenuHotKey* hotKey;
			int size;
			switch (currentSymbol) {
			case TOKEN_AND:
				break;
			case TOKEN_OR:
				size = inputs.size();
				hotKeyInputs = new ButtonInput[size];
				for (int inp = 0; inp < size; inp++) {
					hotKeyInputs[inp] = inputs.at(inp);
				}
				hotKey = new MenuHotKey{ hotKeyInputs, size };
				hotkeys.push_back(hotKey);
				break;
			default:
				return NULL;
			}
			state = 0;
		}
	}
	if (state != 2) {
		return NULL;
	}
	const int size = inputs.size();
	ButtonInput* hotKeyInputs = new ButtonInput[size];
	for (int inp = 0; inp < size; inp++) {
		hotKeyInputs[inp] = inputs.at(inp);
	}
	MenuHotKey* hotKey = new MenuHotKey{ hotKeyInputs, size };
	hotkeys.push_back(hotKey);

	const int numHotkeys = hotkeys.size();
	MenuHotKey** result = new MenuHotKey* [numHotkeys];
	for (i = 0; i < numHotkeys; i++) {
		result[i] = hotkeys.at(i);
	}
	return result;
}

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

bool inputActive(ButtonInput input, DWORD buttonStates[3]) {
	return buttonStates[input.inputType] & input.button;
}

bool hotKeyActivated(const MenuHotKey* hk, DWORD buttonStates[3]) {
	const int numInputs = hk->numInputs;
	const ButtonInput* inputs = hk->inputs;
	for (int i = 0; i < hk->numInputs; i++) {
		if (!inputActive(inputs[i], buttonStates)) {
			return false;
		}
	}
	return true;
}

CheatMenu::CheatMenu(CheatMenuItem** menuItems, int numMenuItems, MenuStyle* style, CheatMenuHotKeys* hotKeys) {
	this->menuItems = menuItems;
	this->numMenuItems = numMenuItems;
	selectedMenuItem = menuItems[0];
	selectedMenuItemIndex = 0;
	this->style = style;
	this->hotKeys = hotKeys;
	showing = false;
}

void CheatMenu::releaseD3DObjects() {
	if (style) {
		if (style->pFont) {
			style->pFont->Release();
		}
		if (style->pSprite) {
			style->pSprite->Release();
		}
	}
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

void CheatMenu::handleInput(DWORD buttonsHeld, DWORD buttonsPressed, DWORD buttonsReleased) {
	DWORD buttonStates[3] = { buttonsHeld, buttonsPressed, buttonsReleased };
	if (showing) {
		if (hotKeyActivated(hotKeys->menuActivate, buttonStates)) {
			selectedMenuItem->onActivate();
		}
		else if (hotKeyActivated(hotKeys->menuLeft, buttonStates)) {
			selectedMenuItemLeft();
		}
		else if (hotKeyActivated(hotKeys->menuRight, buttonStates)) {
			selectedMenuItemRight();
		}
		else if (hotKeyActivated(hotKeys->menuUp, buttonStates)) {
			menuUp();
		}
		else if (hotKeyActivated(hotKeys->menuDown, buttonStates)) {
			menuDown();
		}
		else if (hotKeyActivated(hotKeys->menuClose, buttonStates)) {
			showing = false;
		}
	}
	else {
		if (hotKeyActivated(hotKeys->menuUp, buttonStates) ||
			hotKeyActivated(hotKeys->menuDown, buttonStates) ||
			hotKeyActivated(hotKeys->menuLeft, buttonStates) ||
			hotKeyActivated(hotKeys->menuRight, buttonStates)) {

			showing = true;

		}
	}
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