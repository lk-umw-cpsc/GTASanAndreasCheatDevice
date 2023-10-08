#include "GTA.h"
#include "GTAQuickMenu.h"
#include <string>

using namespace std;

GTASAQuickMenu::GTASAQuickMenu(QuickMenuItem** menuItems, int numMenuItems) : QuickMenu(menuItems, numMenuItems) {
}

void GTASAQuickMenu::show(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT pFont, LPD3DXSPRITE pSprite) {
	if (numMenuItems == 0) {
		return;
	}
	RECT r = { 100, 100, 1000, 300 };
	D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255);
	D3DCOLOR color2 = D3DCOLOR_ARGB(255, 203, 99, 255);
	int fontSize, lineGap;
	//D3DXSPRITE_BILLBOARD | 
	bool selectedCheat;
	D3DCOLOR chosenColor;
	RECT bounds = { 0, 0, 0, 0 };
	int widest = 0;

	pFont->DrawText(pSprite, menuItems[0]->getText().c_str(), -1, &bounds, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));
	widest = bounds.right - bounds.left;
	fontSize = bounds.bottom - bounds.top;
	lineGap = ceil(.2 * fontSize);
	for (int i = 0; i < numMenuItems; i++) {
		pFont->DrawText(pSprite, menuItems[i]->getText().c_str(), -1, &bounds, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));
		int width = bounds.right - bounds.left;
		if (widest < width) {
			widest = width;
		}
	}
	D3DRECT menuBackgroundBounds = { 100 - 25, 100 - 25, 25 + 100 + widest, 25 + 100 + numMenuItems * (fontSize + lineGap) };
	pDevice->Clear(1, &menuBackgroundBounds, D3DCLEAR_TARGET, MENU_BACKGROUND_COLOR, 0, 0);
	
	for (int i = 0; i < numMenuItems; i++) {
		selectedCheat = i == selectedMenuItemIndex;
		if (selectedCheat) {
			chosenColor = MENU_SELECTED_TEXT_COLOR;
		}
		else {
			chosenColor = MENU_TEXT_COLOR;
		}
		pFont->DrawText(pSprite, menuItems[i]->getText().c_str(), -1, &r, DT_LEFT, chosenColor);
		r.top += fontSize + lineGap;
		r.bottom += fontSize + lineGap;
	}
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

string SelfDestructMenuItem::getText() {
	return "Self Destruct";
}

void SelfDestructMenuItem::onActivate() {
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

void StepForwardMenuItem::onActivate() {
	Pedestrian* player = getPlayer();
	Vehicle* vehicle = getCurrentVehicle();
	if (vehicle->baseAddress) {
		vehicle->position->x += vehicle->forward->x * 5;
		vehicle->position->y += vehicle->forward->y * 5;
		vehicle->position->z += vehicle->forward->z * 5;
		vehicle->velocity->x = 0;
		vehicle->velocity->y = 0;
		vehicle->velocity->z = 0;
	}
	else {
		player->position->x += player->forward->x * 5;
		player->position->y += player->forward->y * 5;
		player->position->z += player->forward->z * 5;
		player->velocity->x = 0;
		player->velocity->y = 0;
		player->velocity->z = 0;
	}
}

string StepForwardMenuItem::getText() {
	return "Step forward 5m";
}

void StepUpMenuItem::onActivate() {
	Pedestrian* player = getPlayer();
	Vehicle* vehicle = getCurrentVehicle();
	if (vehicle->baseAddress) {
		vehicle->position->x += vehicle->up->x * 5;
		vehicle->position->y += vehicle->up->y * 5;
		vehicle->position->z += vehicle->up->z * 5;
		vehicle->velocity->x = 0;
		vehicle->velocity->y = 0;
		vehicle->velocity->z = 0;
	}
	else {
		player->position->x += player->up->x * 5;
		player->position->y += player->up->y * 5;
		player->position->z += player->up->z * 5;
		player->velocity->x = 0;
		player->velocity->y = 0;
		player->velocity->z = 0;
	}
}

string StepUpMenuItem::getText() {
	return "Step up 5m";
}

void StepDownMenuItem::onActivate() {
	Pedestrian* player = getPlayer();
	Vehicle* vehicle = getCurrentVehicle();
	if (vehicle->baseAddress) {
		vehicle->position->x -= vehicle->up->x * 5;
		vehicle->position->y -= vehicle->up->y * 5;
		vehicle->position->z -= vehicle->up->z * 5;
		vehicle->velocity->x = 0;
		vehicle->velocity->y = 0;
		vehicle->velocity->z = 0;
	}
	else {
		player->position->x -= player->up->x * 5;
		player->position->y -= player->up->y * 5;
		player->position->z -= player->up->z * 5;
		player->velocity->x = 0;
		player->velocity->y = 0;
		player->velocity->z = 0;
	}
}

string StepDownMenuItem::getText() {
	return "Step down 5m";
}

void QuickTakeOffMenuItem::onActivate() {
	Pedestrian* player = getPlayer();
	Vehicle* vehicle = getCurrentVehicle();
	if (vehicle->baseAddress) {
		vehicle->position->z += 250;
		vehicle->velocity->x += vehicle->forward->x * 5;
		vehicle->velocity->y += vehicle->forward->y * 5;
		vehicle->velocity->z += vehicle->forward->z * 5;
	}
	else {
		player->position->z += 250;
		player->velocity->x += player->forward->x * 5;
		player->velocity->y += player->forward->y * 5;
		player->velocity->z += player->forward->z * 5;
	}
}

string QuickTakeOffMenuItem::getText() {
	return "Quick Take Off";
}