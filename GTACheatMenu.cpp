#include "GTA.h"
#include "GTACheatMenu.h"
#include <string>
#include <sstream>

using namespace std;

GTASACheatMenu::GTASACheatMenu(CheatMenuItem** menuItems, int numMenuItems, MenuStyle* style) : CheatMenu(menuItems, numMenuItems, style) {

}

void GTASACheatMenu::show(LPDIRECT3DDEVICE9 pDevice) {
	if (numMenuItems == 0) {
		return;
	}
	D3DVIEWPORT9 viewport;
	pDevice->GetViewport(&viewport);
	if (!style->pFont) {
		int height = viewport.Height;
		int fffontSize = (int)ceil(style->fontSize * height);
		D3DXCreateFont(pDevice, fffontSize, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Franklin Gothic"), &(style->pFont));
		D3DXCreateSprite(pDevice, &(style->pSprite));
	}
	LPD3DXFONT pFont = style->pFont;
	LPD3DXSPRITE pSprite = style->pSprite;
	int screenWidth = viewport.Width;
	int screenHeight = viewport.Height;
	int fontSize, lineGap;
	bool drawingSelectedCheat;
	D3DCOLOR chosenColor;
	RECT measurement = { 0, 0, 0, 0 };
	int menuWidth = 0;

	pSprite->Begin(D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_ALPHABLEND);

	pFont->DrawText(pSprite, menuItems[0]->getText().c_str(), -1, &measurement, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));
	menuWidth = measurement.right - measurement.left;
	fontSize = measurement.bottom - measurement.top;
	lineGap = (int)ceil(style->lineSpacing * fontSize);
	for (int i = 0; i < numMenuItems; i++) {
		pFont->DrawText(pSprite, menuItems[i]->getText().c_str(), -1, &measurement, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));
		int width = measurement.right - measurement.left;
		if (menuWidth < width) {
			menuWidth = width;
		}
	}
	menuWidth += 2 * style->horizontalPadding * screenWidth;
	int menuHeight = (fontSize + lineGap) * numMenuItems + style->verticalPadding * 2 * screenHeight;
	int menuX = 0, menuY = 0;
	switch (style->horizontalAlignment) {
	case HORIZONTAL_ANCHOR_LEFT:
		menuX = style->horizontalMargin * screenWidth;
		break;
	case HORIZONTAL_ANCHOR_CENTER:
		menuX = (screenWidth - menuWidth) / 2;
		break;
	case HORIZONTAL_ANCHOR_RIGHT:
		menuX = screenWidth - menuWidth - style->horizontalMargin * screenWidth;
	}
	switch (style->verticalAlignment) {
	case VERTICAL_ANCHOR_TOP:
		menuY = style->verticalMargin * screenHeight;
		break;
	case VERTICAL_ANCHOR_CENTER:
		menuY = (screenHeight - menuHeight) / 2;
		break;
	case VERTICAL_ANCHOR_BOTTOM:
		menuY = screenHeight - menuHeight - style->verticalMargin * screenHeight;
	}
	D3DRECT menuBackgroundBounds = { menuX, menuY, menuX + menuWidth, menuY + menuHeight };
	pDevice->Clear(1, &menuBackgroundBounds, D3DCLEAR_TARGET, MENU_BACKGROUND_COLOR, 0, 0);

	RECT drawLocation = { menuX + style->horizontalPadding * screenWidth, menuY + style->verticalPadding * screenHeight, screenWidth, screenHeight };
	for (int i = 0; i < numMenuItems; i++) {
		drawingSelectedCheat = i == selectedMenuItemIndex;
		if (menuItems[i]->canBeActivated()) {
			if (drawingSelectedCheat) {
				chosenColor = MENU_SELECTED_TEXT_COLOR;
			}
			else {
				chosenColor = MENU_TEXT_COLOR;
			}
		}
		else {
			if (drawingSelectedCheat) {
				chosenColor = MENU_SELECTED_DEACTIVATED_TEXT_COLOR;
			}
			else {
				chosenColor = MENU_DEACTIVATED_TEXT_COLOR;
			}
		}
		pFont->DrawText(pSprite, menuItems[i]->getText().c_str(), -1, &drawLocation, DT_LEFT, chosenColor);
		drawLocation.top += fontSize + lineGap;
	}
	pSprite->End();
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
		return "INVALID ID";
	}
}

string RepairVehicleMenuItem::getText()
{
	return "Repair Vehicle";
}

bool RepairVehicleMenuItem::canBeActivated() {
	return vehicle.baseAddress != NULL;
}

void RepairVehicleMenuItem::onActivate()
{
	if (!vehicle.baseAddress) {
		return;
	}
	DWORD** vtable = reinterpret_cast<DWORD**>(vehicle.baseAddress);
	DWORD pRepairVehicle = (*vtable)[50];
	voidObjectFunction repairVehicle = (voidObjectFunction)pRepairVehicle;
	repairVehicle(vehicle.baseAddress);
}

string SelfDestructMenuItem::getText() {
	return "Self Destruct";
}

void SelfDestructMenuItem::onActivate() {
	if (!vehicle.baseAddress) {
		return;
	}
	unsigned int** vtable = reinterpret_cast<unsigned int**>(vehicle.baseAddress);
	unsigned int pBlowUpVehicle = (*vtable)[41];
	voidObjectFunctionDWORDDWORD blowUpVehicle = (voidObjectFunctionDWORDDWORD)pBlowUpVehicle;
	blowUpVehicle(vehicle.baseAddress, 0, 0);
}

bool SelfDestructMenuItem::canBeActivated() {
	return vehicle.baseAddress != NULL;
}

void StepForwardMenuItem::onActivate() {
	if (vehicle.baseAddress) {
		vehicle.position->x += vehicle.forward->x * 5;
		vehicle.position->y += vehicle.forward->y * 5;
		vehicle.position->z += vehicle.forward->z * 5;
		vehicle.velocity->x = 0;
		vehicle.velocity->y = 0;
		vehicle.velocity->z = 0;
	}
	else {
		player.position->x += player.forward->x * 5;
		player.position->y += player.forward->y * 5;
		player.position->z += player.forward->z * 5;
		player.velocity->x = 0;
		player.velocity->y = 0;
		player.velocity->z = 0;
	}
}

string StepForwardMenuItem::getText() {
	return "Step forward 5m";
}

void StepUpMenuItem::onActivate() {
	if (vehicle.baseAddress) {
		vehicle.position->x += vehicle.up->x * 5;
		vehicle.position->y += vehicle.up->y * 5;
		vehicle.position->z += vehicle.up->z * 5;
		vehicle.velocity->x = 0;
		vehicle.velocity->y = 0;
		vehicle.velocity->z = 0;
	}
	else {
		player.position->x += player.up->x * 5;
		player.position->y += player.up->y * 5;
		player.position->z += player.up->z * 5;
		player.velocity->x = 0;
		player.velocity->y = 0;
		player.velocity->z = 0;
	}
}

string StepUpMenuItem::getText() {
	return "Step up 5m";
}

void StepDownMenuItem::onActivate() {
	if (vehicle.baseAddress) {
		vehicle.position->x -= vehicle.up->x * 5;
		vehicle.position->y -= vehicle.up->y * 5;
		vehicle.position->z -= vehicle.up->z * 5;
		vehicle.velocity->x = 0;
		vehicle.velocity->y = 0;
		vehicle.velocity->z = 0;
	}
	else {
		player.position->x -= player.up->x * 5;
		player.position->y -= player.up->y * 5;
		player.position->z -= player.up->z * 5;
		player.velocity->x = 0;
		player.velocity->y = 0;
		player.velocity->z = 0;
	}
}

string StepDownMenuItem::getText() {
	return "Step down 5m";
}

void QuickTakeOffMenuItem::onActivate() {
	if (vehicle.baseAddress) {
		vehicle.position->z += 250;
		vehicle.velocity->x += vehicle.forward->x * 5;
		vehicle.velocity->y += vehicle.forward->y * 5;
		vehicle.velocity->z += vehicle.forward->z * 5;
	}
	else {
		player.position->z += 250;
		player.velocity->x += player.forward->x * 5;
		player.velocity->y += player.forward->y * 5;
		player.velocity->z += player.forward->z * 5;
	}
}

string QuickTakeOffMenuItem::getText() {
	return "Quick Take Off";
}

int VehicleColorMenuItem::getCurrentVehicleColor() {
	if (vehicle.baseAddress) {
		return *vehicle.color1;
	}
	return -1;
}

void VehicleColorMenuItem::onLeftInput() {
	if (!vehicle.baseAddress) {
		return;
	}
	byte currentColor = *vehicle.color1;
	currentColor--;
	*vehicle.color1 = currentColor;
	*vehicle.color2 = currentColor;
}

void VehicleColorMenuItem::onRightInput() {
	if (!vehicle.baseAddress) {
		return;
	}
	byte currentColor = *vehicle.color1;
	currentColor++;
	*vehicle.color1 = currentColor;
	*vehicle.color2 = currentColor;
}

string VehicleColorMenuItem::getText() {
	int color = getCurrentVehicleColor();
	if (color < 0) {
		return "Change Vehicle Color";
	}
	return "Vehicle color " + to_string(color);
}

bool VehicleColorMenuItem::canBeActivated() {
	return vehicle.baseAddress != NULL;
}

string KillEveryoneMenuItem::getText() {
	return "Kill Everyone";
}

void KillEveryoneMenuItem::onActivate() {
	const EntityTable table = **ppPedestrianTable;
	int numSlots = table.numSlots;
	unsigned int pedestrianArrayBase = table.arrayBaseAddress;
	byte* slotInUse = table.slotInUse;
	for (int i = 1; i < numSlots; i++) {
		if (slotInUse[i] >> 7) {
			continue;
		}
		DWORD pedestrianBA = pedestrianArrayBase + i * PEDESTRIAN_OBJECT_SIZE;
		Pedestrian ped;
		hookPedestrian(pedestrianBA, &ped);
		if (*ped.health <= 0) {
			continue;
		}
		ped.position->z += 4.5f;
		ped.velocity->z = -4.f;
		*(BYTE*)(pedestrianBA + 0x46C) = 0;
	}
}

string BlowUpAllVehicles::getText() {
	return "Blow Up All Vehicles";
}

void BlowUpAllVehicles::onActivate() {
	const EntityTable table = **ppVehicleTable;
	int numSlots = table.numSlots;
	unsigned int arrayBaseAddress = table.arrayBaseAddress;
	byte* slotInUse = table.slotInUse;
	unsigned int playerCarBaseAddress = vehicle.baseAddress;
	for (int i = 0; i < numSlots; i++) {
		if (slotInUse[i] >> 7) {
			continue;
		}
		unsigned int baseAddress = arrayBaseAddress + i * VEHICLE_OBJECT_SIZE;
		if(baseAddress == playerCarBaseAddress) {
			continue;
		}
		unsigned int** vtable = reinterpret_cast<unsigned int**>(baseAddress);
		unsigned int pBlowUpVehicle = (*vtable)[41];
		voidObjectFunctionDWORDDWORD blowUpVehicle = (voidObjectFunctionDWORDDWORD)(pBlowUpVehicle);
		blowUpVehicle(baseAddress, 0, 0);
	}
}

ChangeGravityMenuItem::ChangeGravityMenuItem() {
	defaultGravity = *gravity;
	DWORD oldProtections;
	VirtualProtect(gravity, sizeof(float), PAGE_READWRITE, &oldProtections);
}

string ChangeGravityMenuItem::getText() {
	ostringstream out;
	out.precision(3);
	out << fixed << *gravity;
	return "Gravity: " + move(out).str() + " (A to reset)";
}

void ChangeGravityMenuItem::onLeftInput() {
	*gravity -= 0.001f;
}

void ChangeGravityMenuItem::onRightInput() {
	*gravity += 0.001f;
}

void ChangeGravityMenuItem::onActivate() {
	*gravity = defaultGravity;
}

BunnyHopMenuItem::BunnyHopMenuItem() {
	defaultMultiplier = 1.0f;
	*bunnyHopMultiplier = 1.0f;
}

string BunnyHopMenuItem::getText() {
	ostringstream out;
	out.precision(1);
	out << fixed << *bunnyHopMultiplier;
	return "Bunny Hop Multiplier: " + move(out).str() + " (A to reset)";
}

void BunnyHopMenuItem::onLeftInput() {
	*bunnyHopMultiplier -= .5f;
}

void BunnyHopMenuItem::onRightInput() {
	*bunnyHopMultiplier += .5f;
}

void BunnyHopMenuItem::onActivate() {
	*bunnyHopMultiplier = *bunnyHopMultiplier;
}