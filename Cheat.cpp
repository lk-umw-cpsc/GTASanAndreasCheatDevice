#include "Cheat.h"

Cheat::Cheat(void (*onEnable)(), void (*onDisable)(), void (*onFrame)()) {
	enabled = false;
	this->onEnable = onEnable;
	this->onDisable = onDisable;
	this->onFrame = onFrame;
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