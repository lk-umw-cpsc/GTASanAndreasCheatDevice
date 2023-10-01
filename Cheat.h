#pragma once
#include <string>
using namespace std;

class Cheat {
private:
	bool enabled;
	string name;
public:
	Cheat(void (*onEnable)(), void (*onDisable)(), void (*onFrame)(), string name);
	void (*onEnable)();
	void (*onFrame)();
	void (*onDisable)();
	bool isEnabled();
	void setEnabled(bool enabled);
	void toggle();
	string getMenuText();
};