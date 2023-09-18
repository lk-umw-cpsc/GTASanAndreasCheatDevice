#include "Cheat.h"

void Cheat::setEnabled(bool enabled) {
	this->enabled = enabled;
}

void Cheat::toggle() {
	this->enabled = !(this->enabled);
}