#include "GTA.h"
#include "GTAQuickMenu.h"
#include <string>

using namespace std;

GTASAQuickMenu::GTASAQuickMenu(QuickMenuItem** menuItems, int numMenuItems) : QuickMenu(menuItems, numMenuItems) {
}

void GTASAQuickMenu::show() {
	string menuText = MENU_WHITE_TEXT_TOKEN;

	const int last = numMenuItems - 1;

	for (int i = 0; i < numMenuItems; i++) {
		bool onSelected = i == selectedMenuItemIndex;
		if (onSelected) {
			menuText += MENU_PURPLE_TEXT_TOKEN;
		}

		menuText += menuItems[i]->getText();
		if (i != last) {
			menuText += MENU_NEW_LINE;
		}
		if (onSelected) {
			menuText += MENU_WHITE_TEXT_TOKEN;
		}
	}

	displayMessage(menuText.c_str(), 0, 0, 0);
}

SpawnCarMenuItem::SpawnCarMenuItem(int defaultCarID) {
	selectedCarID = defaultCarID;
}


void SpawnCarMenuItem::onActivate()
{
	spawnCar(selectedCarID);
}

void SpawnCarMenuItem::onLeftInput() {
	selectedCarID--;
	if (selectedCarID <= 399) {
		selectedCarID = 611;
	}
	bool badID = false;
	do {
		switch (selectedCarID) {
		case 449:
		case 537:
		case 538:
		case 569:
		case 570:
		case 590:
		case 594:
			selectedCarID--;
			badID = true;
			break;
		default:
			badID = false;
		}
	} while (badID);
}

void SpawnCarMenuItem::onRightInput() {
	selectedCarID++;
	if (selectedCarID >= 612) {
		selectedCarID = 400;
	}
	bool badID = false;
	do {
		switch (selectedCarID) {
		case 449:
		case 537:
		case 538:
		case 569:
		case 570:
		case 590:
		case 594:
			selectedCarID++;
			badID = true;
			break;
		default:
			badID = false;
		}
	} while (badID);
}

string SpawnCarMenuItem::getText() {
	return "Spawn " + getSelectedVehicleName() + " (" + to_string(selectedCarID) + ")";
}

string SpawnCarMenuItem::getSelectedVehicleName() {
	unsigned int address = entityInfo[selectedCarID];
	string text = "";
	if (address) {
		char* nameString = reinterpret_cast<char*>(address + 0x32);
		return nameString;
	}
	else {
		return "Can't spawn: Car " + to_string(selectedCarID) + " is invalid";
	}
}

string RepairVehicleMenuItem::getText()
{
	return "Repair Vehicle";
}

void RepairVehicleMenuItem::onActivate()
{
	Vehicle* vehicle = getCurrentVehicle();
	unsigned int vehicleBaseAddress = vehicle->baseAddress;
	if (vehicleBaseAddress == 0) {
		return;
	}
	unsigned int** vtable = reinterpret_cast<unsigned int**>(vehicleBaseAddress);
	unsigned int pRepairVehicle = (*vtable)[50];
	void(*repairVehicle)() = (void(*)())(pRepairVehicle);
	__asm { mov ecx, [vehicleBaseAddress] }
	repairVehicle();
}

string BlowUpMenuItem::getText() {
	return "Self Destruct";
}

void BlowUpMenuItem::onActivate() {
	Vehicle* vehicle = getCurrentVehicle();
	unsigned int vehicleBaseAddress = vehicle->baseAddress;
	if (!vehicleBaseAddress) {
		return;
	}
	unsigned int** vtable = reinterpret_cast<unsigned int**>(vehicleBaseAddress);
	unsigned int pBlowUpVehicle = (*vtable)[41];
	void(*blowUpVehicle)(unsigned int, unsigned int) = (void(*)(unsigned int, unsigned int))(pBlowUpVehicle);
	__asm { mov ecx, [vehicleBaseAddress] }
	blowUpVehicle(0, 0);
}