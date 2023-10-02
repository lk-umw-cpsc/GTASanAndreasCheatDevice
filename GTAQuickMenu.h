#pragma once

#include "QuickMenu.h"
#include "dllMain.h"

#define MENU_NEW_LINE "~N~"
#define MENU_WHITE_TEXT_TOKEN "~w~"
#define MENU_PURPLE_TEXT_TOKEN "~p~"

class GTASAQuickMenu : public QuickMenu {
public:
	GTASAQuickMenu(QuickMenuItem** menuItems, int numMenuItems);
	void show();
};

class SpawnCarMenuItem : public QuickMenuItem {
private:
	int selectedCarID;
public:
	SpawnCarMenuItem(int defaultCarID);
	string getText();
	void onLeftInput();
	void onRightInput();
	void onActivate();
	string getSelectedVehicleName();
};

class RepairVehicleMenuItem : public QuickMenuItem {
public:
	string getText();
	void onActivate();
};

class BlowUpMenuItem : public QuickMenuItem {
public:
	string getText();
	void onActivate();
};