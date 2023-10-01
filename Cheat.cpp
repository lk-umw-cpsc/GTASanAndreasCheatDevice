#include "Cheat.h"
#include <string>

Cheat::Cheat(void (*onEnable)(), void (*onDisable)(), void (*onFrame)(), string name) {
	enabled = false;
	this->onEnable = onEnable;
	this->onDisable = onDisable;
	this->onFrame = onFrame;
	this->name = name;
}

void Cheat::setEnabled(bool enabled) {
	bool currentState = this->enabled;
	if (currentState != enabled) {
		this->enabled = enabled;
		if (enabled && onEnable) {
			onEnable();
		} else if(!enabled && onDisable) {
			onDisable();
		}
	}
}

bool Cheat::isEnabled() {
	return enabled;
}

void Cheat::toggle() {
	enabled = !enabled;
	if (enabled && onEnable) {
		onEnable();
	} else if (!enabled && onDisable) {
		onDisable();
	}
}

string Cheat::getMenuText() {
	string onOff;
	if (enabled) {
		onOff = " ON";
	}
	else {
		onOff = " OFF";
	}
	return name + onOff;
}