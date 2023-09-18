#pragma once

class Cheat {
private:
	bool enabled;

public:
	Cheat(void (*onEnable)(), void (*onDisable)(), void (*onFrame)());
	void (*onEnable)();
	void (*onFrame)();
	void (*onDisable)();
	bool isEnabled();
	void setEnabled(bool enabled);
	void toggle();
};