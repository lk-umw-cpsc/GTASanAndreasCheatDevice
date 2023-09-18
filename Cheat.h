#pragma once

class Cheat {
private:
	bool enabled;

public:
	void (*trigger)();
	void setEnabled(bool enabled);
	void toggle();
};