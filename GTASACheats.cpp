#include <math.h>
#include <chrono>

#include "Memory.h"
#include "GTA.h"
#include "dllMain.h"

#include "GTASACheats.h"

long long getTime() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void noWantedLevel() {
	*pNoCrimesFlag = 1;
	*wantedLevel.heat = 0;
	*wantedLevel.stars = 0;
}

void disableNoWantedLevel() {
	*pNoCrimesFlag = 0;
}

void infiniteHealth() {
	*player.health = 999.f;
	*player.armor = 999.f;
	*playerOxygen = 3000.f;
}

void infiniteHealthOff() {
	*player.health = 100.f;
	*player.armor = 100.f;
	*playerOxygen = 1000.f;
}

void infiniteCarHealth() {
	if (!vehicle.baseAddress) {
		return;
	}
	*vehicle.health = 10000.f;
	if (vehicle.objectClass == CLASS_CAR) {
		*vehicle.wheelState = { 0, 0, 0, 0 };
	}
}

#define HOVER_CAR_MAX_VELOCITY 3.0f
#define HOVER_CAR_ACCELERATION 0.1f
#define HOVER_CAR_FRICTION 0.1f
#define HOVER_CAR_FORWARD_FORCE (1.0f - HOVER_CAR_FRICTION)
void hoverCar() {
	if (!vehicle.baseAddress) {
		return;
	}
	if (vehicle.objectClass == CLASS_CAR) {
		float tireRot = *vehicle.tireRotation;
		if (tireRot < 2.0f) {
			*vehicle.tireRotation = tireRot + 0.05f;
		}
		else if (tireRot > 2.0f) {
			*vehicle.tireRotation = 2.0f;
		}
	}
	bool accelerate_button_held;
	bool reverse_button_held;
	bool lift_button_held;
	bool punt_button_pressed;
	bool hover_car_toggle_button_pressed;
	bool rocket_button_pressed = false;
	bool rotate_car_button_held = false;
	float left_stick_x = 0.0f, left_stick_y = 0.0f;
	float right_stick_x = 0.0f, right_stick_y = 0.0f;
	if (usingGamepad) {
		accelerate_button_held = buttonsHeld & XINPUT_GAMEPAD_A;
		reverse_button_held = buttonsHeld & XINPUT_GAMEPAD_X;
		lift_button_held = buttonsHeld & XINPUT_GAMEPAD_B;
		punt_button_pressed = buttonsHeld & XINPUT_GAMEPAD_DPAD_UP;
		hover_car_toggle_button_pressed = buttonsPressed & XINPUT_GAMEPAD_DPAD_DOWN;
		rocket_button_pressed = buttonsPressed & XINPUT_GAMEPAD_LEFT_THUMB;
		rotate_car_button_held = buttonsHeld & XINPUT_GAMEPAD_RIGHT_SHOULDER;

		if (abs(gamepadState.Gamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			left_stick_x = gamepadState.Gamepad.sThumbLX / (float)(32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		if (abs(gamepadState.Gamepad.sThumbLY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			left_stick_y = gamepadState.Gamepad.sThumbLY / (float)(32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		if (abs(gamepadState.Gamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			right_stick_x = gamepadState.Gamepad.sThumbRX / (float)(32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		if (abs(gamepadState.Gamepad.sThumbRY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			right_stick_y = gamepadState.Gamepad.sThumbRY / (float)(32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	}
	else {
		return;
	}

	float rx = vehicle.forward->x;
	float ry = vehicle.forward->y;
	float rv = (float)sqrt(rx * rx + ry * ry);
	rx /= rv;
	ry /= rv;

	*vehicle.forward = { rx, ry, 0.0f };
	*vehicle.left = { ry, -rx, 0.0f };
	*vehicle.up = { 0.0f, 0.0f, 1.0f };
	vehicle.rotationalVelocity->x = 0.0f;
	vehicle.rotationalVelocity->y = 0.0f;

	if (accelerate_button_held) {
		Vector3d vehicleVelocity = *vehicle.velocity;
		float velocity = distance2d(vehicleVelocity.x, vehicleVelocity.y);
		velocity = min(velocity, HOVER_CAR_MAX_VELOCITY);
		vehicle.velocity->x = vehicle.forward->x * velocity * HOVER_CAR_FORWARD_FORCE + vehicle.velocity->x * HOVER_CAR_FRICTION + vehicle.forward->x * HOVER_CAR_ACCELERATION;
		vehicle.velocity->y = vehicle.forward->y * velocity * HOVER_CAR_FORWARD_FORCE + vehicle.velocity->y * HOVER_CAR_FRICTION + vehicle.forward->y * HOVER_CAR_ACCELERATION;
	}
	else if (reverse_button_held) {
		vehicle.velocity->x = vehicle.forward->x * -0.8f;
		vehicle.velocity->y = vehicle.forward->y * -0.8f;
	}
	else {
		vehicle.velocity->x *= 0.9f;
		vehicle.velocity->y *= 0.9f;
	}
	if (left_stick_y != 0.0f) {
		vehicle.velocity->z = 0.8f * left_stick_y;
	}
	else {
		//if (car->wheels[0] || car->wheels[1] || car->wheels[2] || car->wheels[3]) {
			//car->velocity.z = 0.019f;
		//} else {

		//}
		vehicle.velocity->z = 0.013f;
	}
	if (left_stick_x) {
		vehicle.rotationalVelocity->z = 0.05f * -left_stick_x;
	}
	else {
		vehicle.rotationalVelocity->z *= 0.5f;
	}
}

void disableHoverCar() {
	if (!vehicle.baseAddress || vehicle.objectClass != CLASS_CAR) {
		return;
	}
	*vehicle.tireRotation = 0.7046132684f;
}

void autoFlipCar() {
	if (!vehicle.baseAddress || vehicle.objectClass != CLASS_CAR) {
		return;
	}
	if (vehicle.up->z < -0.2f) {
		if (vehicle.rotationalVelocity->y > 0)
			vehicle.rotationalVelocity->y = 0.05f;
		else
			vehicle.rotationalVelocity->y = -0.05f;
	}
}

BYTE originalFallOffBikeCall[5];
void installFallOffBikeDetour() {
	DWORD newJumpAddress;
	newJumpAddress = (DWORD)&fallOffBikeDetour - 0x4b4ac0 - 0x05;
	BYTE detour[] = { 0xE9, 0, 0, 0, 0 }; //E9 for jmp
	memcpy((LPVOID)(detour + 1), (LPVOID)&newJumpAddress, sizeof(newJumpAddress));
	overwriteInstructions((void*)0x4b4ac0, detour, sizeof(detour), originalFallOffBikeCall);
}

void fallOffBikeDetour() {
	__asm {
		push ecx
		mov ecx, 0xb6f5f0 //pplayer
		mov ecx, [ecx]
		cmp eax, ecx
		pop ecx
		jne notplayer
		mov al, 0
		ret 0004
		notplayer:
		sub esp, 0x1C
			push ebx
			push esi
			push 0x4b4ac5 // code immediately after replaced code
			ret
	}
}

void uninstallFallOffBikeDetour() {
	restoreInstructions((void*)0x4b4ac0, originalFallOffBikeCall, sizeof(originalFallOffBikeCall));
}

const DWORD infiniteAmmoNOPTargets[4] = {
	WEAPON_AMMO_DECREMENT1_ADDRESS,
	WEAPON_AMMO_DECREMENT2_ADDRESS,
	WEAPON_AMMO_DECREMENT3_ADDRESS,
	WEAPON_AMMO_DECREMENT4_ADDRESS,
};
DWORD originalAmmoDecrementCode[4][3] = { 0 };
#define NUM_AMMO_DECREMENT_OP_CODES (sizeof(infiniteAmmoNOPTargets) / sizeof(DWORD))
void enableInfiniteAmmo() {
	BYTE nops[WEAPON_AMMO_DECREMENT_SIZE] = { 0x90, 0x90, 0x90 };
	for (int i = 0; i < NUM_AMMO_DECREMENT_OP_CODES; i++) {
		overwriteInstructions((void*)infiniteAmmoNOPTargets[i], nops, WEAPON_AMMO_DECREMENT_SIZE, originalAmmoDecrementCode[i]);
	}
}

void disableInfiniteAmmo() {
	for (int i = 0; i < NUM_AMMO_DECREMENT_OP_CODES; i++) {
		restoreInstructions((void*)infiniteAmmoNOPTargets[i], originalAmmoDecrementCode[i], WEAPON_AMMO_DECREMENT_SIZE);
	}
}

BYTE originalCarDamageCall[5];
BYTE disablePlaneBlowUpInstructions[2] = { 0xeb, 0x0b };
BYTE originalPlaneBlowUpInstructions[2];
void installNoCarDamageDetour() {
	DWORD jumpDestination;
	jumpDestination = (DWORD)&noVehicleDamageDetour - 0x6A7650 - 0x04;
	BYTE detour[] = { 0xE9, 0, 0, 0, 0 }; //E9 for jmp
	memcpy((LPVOID)(detour + 1), (LPVOID)&jumpDestination, sizeof(jumpDestination));
	overwriteInstructions((void*)0x6A7650, detour, sizeof(detour), &originalCarDamageCall);
}

void uninstallNoCarDamageDetour() {
	restoreInstructions((void*)0x6A7650, &originalCarDamageCall, sizeof(originalCarDamageCall));
}

void noCarDamageDetour() {
	__asm {
		push ecx
		push ebx
		mov ecx, 0x00BA18FC
		mov ecx, [ecx]
		mov ebx, [esp + 0x0C]
		cmp ecx, ebx
		pop ebx
		pop ecx
		jne notplayercar
		ret
		notplayercar :
		push ebp
			mov ebp, [esp + 8]
			push 0x6C24B5
			ret
	}
}

void fixCar() {
	DWORD baseAddress = vehicle.baseAddress;
	if (baseAddress && baseAddress != vehicleBaseAddressLastFrame) {
		DWORD** vtable = reinterpret_cast<DWORD**>(baseAddress);
		DWORD pRepairVehicle = (*vtable)[50];
		voidObjectFunction repairVehicle = (voidObjectFunction)pRepairVehicle;
		repairVehicle(baseAddress);
	}
}

void noVehicleDamageDetour() {
	__asm {
		push ebx
		mov ebx, 0x00BA18FC
		mov ebx, [ebx]
		cmp ecx, ebx
		pop ebx
		jne notpcar
		ret 0x18
		notpcar:
		push - 1
			push 0x00847FD8
			push 0x6a7657
			ret
	}
}

BYTE originalCheckVehicleLockOpcodes[5];
void enableEnterAnyVehicle() {
	BYTE jump[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };
	DWORD jumpOffset = (DWORD)&carLockDetour - VEHICLE_LOCK_CHECK_ADDRESS + FAR_JUMP_OFFSET;
	memcpy(&jump[1], &jumpOffset, sizeof(DWORD));
	overwriteInstructions((void*)VEHICLE_LOCK_CHECK_ADDRESS, jump, sizeof(jump), originalCheckVehicleLockOpcodes);
}

void disableEnterAnyVehicle() {
	restoreInstructions((void*)VEHICLE_LOCK_CHECK_ADDRESS, originalCheckVehicleLockOpcodes, sizeof(originalCheckVehicleLockOpcodes));
}

void carLockDetour() {
	// If the driver of the car is the player, DO check the lock. Otherwise, bypass
	__asm {
		push edx
		push ecx
		add ecx, VEHICLE_PASSENGER_OFFSET
		mov ecx, [ecx]
		mov edx, PLAYER_PEDESTRIAN_STRUCT_POINTER
		mov edx, [edx]
		cmp ecx, edx
		pop ecx
		pop edx
		je driver_is_player
		mov al, 1
		pop esi
		ret 4
		driver_is_player:
		xor al, al
			pop esi
			ret 4
	}
}

BYTE noPlaneExplosionInstructions[3] = { 0xC2, 0x08, 0x00 }; // ret 08
BYTE originalPlaneExplosionInstructions[3];
BYTE noPlaneDamageOnCollisionInstructions[2] = { 0xEB, 0x0F };
BYTE originalPlaneDamageOnCollisionInstructions[2];
void enableNoPlaneExplosion() {
	overwriteInstructions((void*)0x006B1F30, noPlaneDamageOnCollisionInstructions, sizeof(noPlaneDamageOnCollisionInstructions), originalPlaneDamageOnCollisionInstructions);
}

void disableNoPlaneExplosion() {
	restoreInstructions((void*)0x006B1F30, originalPlaneDamageOnCollisionInstructions, sizeof(originalPlaneDamageOnCollisionInstructions));
}

void rhinoCar() {
	DWORD vehicleBaseAddress = vehicle.baseAddress;
	if (vehicleBaseAddress) {
		DWORD touchedObjectBaseAddress = *vehicle.pTouch;
		if (vehicleBaseAddress != vehicleBaseAddressLastFrame) {
			*vehicle.mass *= 8;
		}
		if (touchedObjectBaseAddress) {
			Vehicle v;
			hookVehicle(touchedObjectBaseAddress, &v);
			if (*v.health > 0) {
				DWORD* pTouchedObjectClass = reinterpret_cast<DWORD*>(touchedObjectBaseAddress);
				DWORD cls = *pTouchedObjectClass;
				switch (cls) {
				case CLASS_CAR:
				case CLASS_MOTORCYCLE:
				case CLASS_PLANE:
				case CLASS_HELICOPTER:
					DWORD* vtable = reinterpret_cast<DWORD*>(cls);
					DWORD pBlowUpVehicle = vtable[41];
					voidObjectFunctionDWORDDWORD blowUpVehicle = (voidObjectFunctionDWORDDWORD)pBlowUpVehicle;
					blowUpVehicle(touchedObjectBaseAddress, player.baseAddress, 0);
					break;
				}
			}
		}
	}
}

Vehicle* getCurrentVehicle() {
	return &vehicle;
}

Pedestrian* getPlayer() {
	return &player;
}

int afterPuntFrames = 0;
Vector3d pleft, pforward, pup, pposition, pvelocity, protVelocity;
#define PUNT_STEP 5.0f
void bToPunt() {
	if (!vehicle.baseAddress) {
		return;
	}
	if (buttonsPressed & XINPUT_GAMEPAD_B && afterPuntFrames == 0) {
		pleft = *vehicle.left;
		pforward = *vehicle.forward;
		pup = *vehicle.up;
		pposition = *vehicle.position;
		pvelocity = *vehicle.velocity;
		protVelocity = *vehicle.rotationalVelocity;

		vehicle.velocity->x += PUNT_STEP * vehicle.forward->x;
		vehicle.velocity->y += PUNT_STEP * vehicle.forward->y;
		vehicle.velocity->z += PUNT_STEP * vehicle.forward->z;
		*vehicle.mass *= 8;
		*vehicle.impulse = { 0, 0, 0 };
		*vehicle.rotationalImpulse = { 0, 0, 0 };
		afterPuntFrames = 1;
	}
	else if (afterPuntFrames > 0) {
		if (afterPuntFrames == 1) {
			*vehicle.mass /= 8;
		}
		pposition.x += pvelocity.x;
		pposition.y += pvelocity.y;
		pposition.z += pvelocity.z;
		*vehicle.left = pleft;
		*vehicle.forward = pforward;
		*vehicle.up = pup;
		*vehicle.position = pposition;
		*vehicle.velocity = pvelocity;
		*vehicle.rotationalVelocity = protVelocity;
		*vehicle.impulse = { 0, 0, 0 };
		*vehicle.rotationalImpulse = { 0, 0, 0 };
		afterPuntFrames--;
	}
}

byte originalPropertyPurchaseCode[5];
void installPropertyPurchaseDetour() {
	byte jump[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };
	DWORD jumpOffset = (DWORD)&propertyPurchaseDetour - 0x457dd5 + FAR_JUMP_OFFSET;
	memcpy(&jump[1], &jumpOffset, sizeof(DWORD));
	overwriteInstructions((void*)0x457dd5, jump, sizeof(jump), originalPropertyPurchaseCode);
}

void uninstallPropertyPurchaseDetour() {
	restoreInstructions((void*)0x457dd5, originalPropertyPurchaseCode, sizeof(originalPropertyPurchaseCode));
}

void propertyPurchaseDetour() {
	__asm {
		cmp eax, 0x0F
		jne not_0x0F
		mov eax, 0x10
		not_0x0F:
		movzx edx, byte ptr[eax + 0x0045887C]
			jmp dword ptr[edx * 4 + 0x0045885C]
	}
}

byte originalExplorationCrimeInstructions[6];
void enableExploreAnywhere() {
	byte nops[6] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	overwriteInstructions((void*)EXPLORATION_CRIME_INSTRUCTION_ADDRESS, nops, sizeof(nops), originalExplorationCrimeInstructions);
}

void disableExploreAnywhere() {
	restoreInstructions((void*)EXPLORATION_CRIME_INSTRUCTION_ADDRESS, originalExplorationCrimeInstructions, sizeof(originalExplorationCrimeInstructions));
}

void holdRBForAirBrake() {
	if (buttonsHeld & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		if (vehicle.baseAddress) {
			vehicle.velocity->x *= .9f;
			vehicle.velocity->y *= .9f;
			vehicle.velocity->z *= .9f;
		}
		else {
			player.velocity->x *= .9f;
			player.velocity->y *= .9f;
			player.velocity->z *= .9f;
		}
	}
}

void dpadLeftToToggleCarLock() {
	if (!vehicle.baseAddress) {
		return;
	}
	if (vehicle.objectClass == CLASS_BOAT || vehicle.objectClass == CLASS_MOTORCYCLE || vehicle.objectClass == CLASS_BICYCLE) {
		return;
	}
	if (!(buttonsHeld & XINPUT_GAMEPAD_LEFT_SHOULDER || buttonsHeld & XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		if (buttonsPressed & XINPUT_GAMEPAD_DPAD_LEFT) {
			if (*vehicle.lock == VEHICLE_LOCK_STATE_LOCKED) {
				*vehicle.lock = VEHICLE_LOCK_STATE_UNLOCKED;
				displayMessage("Doors unlocked", 0, 0, 0);
			}
			else {
				*vehicle.lock = VEHICLE_LOCK_STATE_LOCKED;
				displayMessage("Doors locked", 0, 0, 0);
			}
		}
	}
}

void autoLockCarDoors() {
	if (vehicle.baseAddress && vehicle.baseAddress != vehicleBaseAddressLastFrame) {
		if (vehicle.objectClass == CLASS_BOAT || vehicle.objectClass == CLASS_MOTORCYCLE || vehicle.objectClass == CLASS_BICYCLE) {
			return;
		}
		// in a new car
		*vehicle.lock = VEHICLE_LOCK_STATE_LOCKED;
		displayMessage("Doors locked", 0, 0, 0);
	}
}

Vector3d rleft, rforward, rup, rposition, rvelocity, rrotVelocity;
int repelRestoreFrames = 0;
void repellantTouch() {
	DWORD touch;
	Vector3d* playerPosition;
	if (vehicle.baseAddress) {
		touch = *vehicle.pTouch;
		playerPosition = vehicle.position;
	}
	else {
		touch = *player.pTouch;
		playerPosition = player.position;
		return;
	}
	if (repelRestoreFrames > 0) {
		rposition.x += rvelocity.x;
		rposition.y += rvelocity.y;
		rposition.z += rvelocity.z;
		*vehicle.left = rleft;
		*vehicle.forward = rforward;
		*vehicle.up = rup;
		*vehicle.position = rposition;
		*vehicle.velocity = rvelocity;
		*vehicle.rotationalVelocity = rrotVelocity;
		repelRestoreFrames--;
	}
	if (!touch) {
		rleft = *vehicle.left;
		rforward = *vehicle.forward;
		rup = *vehicle.up;
		rposition = *vehicle.position;
		rvelocity = *vehicle.velocity;
		rrotVelocity = *vehicle.rotationalVelocity;
	}
	else {
		// bye bye
		DWORD cls = *(DWORD*)touch;
		Vector3d* touchedObjectVelocity = nullptr;
		Vector3d* touchedObjectPosition = nullptr;
		switch (cls) {
		case CLASS_BICYCLE:
		case CLASS_BOAT:
		case CLASS_CAR:
		case CLASS_HELICOPTER:
		case CLASS_PLANE:
		case CLASS_MOTORCYCLE:
			Vehicle v;
			hookVehicle(touch, &v);
			touchedObjectVelocity = v.velocity;
			touchedObjectPosition = v.position;
			break;
		default:
			rleft = *vehicle.left;
			rforward = *vehicle.forward;
			rup = *vehicle.up;
			rposition = *vehicle.position;
			rvelocity = *vehicle.velocity;
			rrotVelocity = *vehicle.rotationalVelocity;
			return;
		}
		float dx = touchedObjectPosition->x - player.position->x;
		float dy = touchedObjectPosition->y - player.position->y;
		float dz = touchedObjectPosition->z - player.position->z;
		float d = distance(dx, dy, dz);
		dx /= d;
		dy /= d;
		dz /= d;
		touchedObjectVelocity->x += dx * 3 + rvelocity.x;
		touchedObjectVelocity->y += dy * 3 + rvelocity.y;
		touchedObjectVelocity->z += dz * 3 + rvelocity.z;
		touchedObjectPosition->x += dx * 1;
		touchedObjectPosition->y += dy * 1;
		touchedObjectPosition->z += dz * 1;
		if (repelRestoreFrames > 0) {
			repelRestoreFrames++;
			return;
		}
		// pretend the collision didn't happen
		rposition.x += rvelocity.x * 2;
		rposition.y += rvelocity.y * 2;
		rposition.z += rvelocity.z * 2;
		*vehicle.left = rleft;
		*vehicle.forward = rforward;
		*vehicle.up = rup;
		*vehicle.position = rposition;
		*vehicle.velocity = rvelocity;
		*vehicle.rotationalVelocity = rrotVelocity;
		*vehicle.impulse = { 0, 0, 0 };
		*vehicle.rotationalImpulse = { 0, 0, 0 };
	}
}

byte originalPlaneDamageCheckInstructions[PLANE_DAMAGE_CHECK_INSTRUCTION_SIZE];
byte originalPlaneFireInstructions[5];
void enableNoPlaneDamage() {
	byte nops[2] = { 0x90, 0x90 };
	byte jump[5] = { 0xe9, 0xc7, 0x00, 0x00, 0x00 };
	overwriteInstructions((void*)PLANE_DAMAGE_CHECK_INSTRUCTION_ADDRESS, nops, sizeof(nops), originalPlaneDamageCheckInstructions);
	overwriteInstructions((void*)0x6cc96f, jump, sizeof(jump), originalPlaneFireInstructions);
	overwriteInstructions((void*)0x6cca2e, disablePlaneBlowUpInstructions, sizeof(disablePlaneBlowUpInstructions), &originalPlaneBlowUpInstructions);
}

void disableNoPlaneDamage() {
	restoreInstructions((void*)PLANE_DAMAGE_CHECK_INSTRUCTION_ADDRESS, originalPlaneDamageCheckInstructions, sizeof(originalPlaneDamageCheckInstructions));
	restoreInstructions((void*)0x6cc96f, originalPlaneFireInstructions, sizeof(originalPlaneFireInstructions));
	restoreInstructions((void*)0x6cca2e, &originalPlaneBlowUpInstructions, sizeof(originalPlaneBlowUpInstructions));
}

void discoMode() {
	const EntityTable table = **ppVehicleTable;
	int numSlots = table.numSlots;
	unsigned int arrayBaseAddress = table.arrayBaseAddress;
	byte* slotInUse = table.slotInUse;
	unsigned int playerCarBaseAddress = vehicle.baseAddress;
	Vehicle v;
	Vector3d playerPosition = *(vehicle.baseAddress ? vehicle.position : player.position);
	Vector3d playerVelocity = *(vehicle.baseAddress ? vehicle.velocity : player.velocity);
	float speed = distance(playerVelocity.x, playerVelocity.y, playerVelocity.z);
	for (int i = 0; i < numSlots; i++) {
		if (slotInUse[i] >> 7) {
			continue;
		}
		unsigned int baseAddress = arrayBaseAddress + i * VEHICLE_OBJECT_SIZE;
		byte randomColor = rand() % 186 + 1;
		*(BYTE*)(baseAddress + VEHICLE_COLOR1_OFFSET) = randomColor;
		*(BYTE*)(baseAddress + VEHICLE_COLOR2_OFFSET) = randomColor;
		/*hookVehicle(baseAddress, &v);
		Vector3d position = *v.position;
		position.x -= playerPosition.x;
		position.y -= playerPosition.y;
		float d2d = distance2d(position.x, position.y);
		if (d2d < 15.f) {
			position.x /= d2d;
			position.y /= d2d;
			v.velocity->x = position.x * speed;
			v.velocity->y = position.y * speed;
		}*/
	}
}

void infiniteNos() {
	if (!vehicle.baseAddress || vehicle.objectClass != CLASS_CAR) {
		return;
	}
	*vehicle.mods |= NOS_ENABLED;
	*vehicle.nosCount = 1;
	if (*vehicle.nosAmount >= 0.f) {
		*vehicle.nosAmount = 1.f;
	}
}

long long dowTimer = 0;

void driveOnWalls() {
	if (!vehicle.baseAddress || vehicle.objectClass == CLASS_BOAT || vehicle.objectClass == CLASS_HELICOPTER) {
		return;
	}
	float gs = *gameSpeed;
	long long now = getTime();
	if (*vehicle.onGroundFlag & 1) {
		dowTimer = now + (long long)(300 / gs);
	}
	if (now < dowTimer) {
		Vector3d* up = vehicle.up;
		Vector3d* velocity = vehicle.velocity;
		velocity->x += -0.013 * up->x * gs;
		velocity->y += -0.013 * up->y * gs;
		velocity->z += -0.013 * up->z * gs + 0.013 * gs;
	}
}

BYTE overwrittenTrippyInstructions[2];
void enableTrippy() {
	BYTE instructions[] = { 0xEB, 0x0F };
	overwriteInstructions((void*)TRIPPY_EFFECT_INSTRUCTION_ADDRESS, instructions, sizeof(instructions), overwrittenTrippyInstructions);
}

void disableTrippy() {
	restoreInstructions((void*)TRIPPY_EFFECT_INSTRUCTION_ADDRESS, overwrittenTrippyInstructions, sizeof(overwrittenTrippyInstructions));
}

void spawnExplosionAtTargetPedestrian() {
	DWORD target = *player.pTargetBaseAddress;
	if (!target || !(buttonsPressed & XINPUT_GAMEPAD_DPAD_RIGHT)) {
		return;
	}
	Pedestrian pedestrian;
	hookPedestrian(target, &pedestrian);
	Vector3d position = *pedestrian.position;
	spawnExplosion(player.baseAddress, 0x13, position.x, position.y, position.z, NULL, NULL, NULL);
}

void goAway() {
	const EntityTable table = **ppVehicleTable;
	int numSlots = table.numSlots;
	unsigned int arrayBaseAddress = table.arrayBaseAddress;
	byte* slotInUse = table.slotInUse;
	unsigned int playerCarBaseAddress = vehicle.baseAddress;
	Vehicle v;
	Vector3d playerPosition = *(vehicle.baseAddress ? vehicle.position : player.position);
	Vector3d playerVelocity = *(vehicle.baseAddress ? vehicle.velocity : player.velocity);
	float speed = distance(playerVelocity.x, playerVelocity.y, playerVelocity.z);
	for (int i = 0; i < numSlots; i++) {
		if (slotInUse[i] >> 7) {
			continue;
		}
		unsigned int baseAddress = arrayBaseAddress + i * VEHICLE_OBJECT_SIZE;
		if (baseAddress == vehicle.baseAddress) {
			continue;
		}
		hookVehicle(baseAddress, &v);
		Vector3d position = *v.position;
		position.x -= playerPosition.x;
		position.y -= playerPosition.y;
		// ba, 198, 42b, 747, 778. 
		float d2d = distance2d(position.x, position.y);
		if (d2d < 15.f) {
			position.x /= d2d;
			position.y /= d2d;
			v.velocity->x = position.x * speed;
			v.velocity->y = position.y * speed;
		}
	}
}

BYTE loseWeaponsOnDeathInstructions[2];
void enableKeepWeaponsOnDeath() {
	BYTE jumpOver[2] = { 0xEB, 0x0B };
	overwriteInstructions((void*)LOSE_WEAPONS_ON_DEATH_INSTRUCTION_ADDRESS, jumpOver, sizeof(jumpOver), loseWeaponsOnDeathInstructions);
}

void disableKeepWeaponsOnDeath() {
	restoreInstructions((void*)LOSE_WEAPONS_ON_DEATH_INSTRUCTION_ADDRESS, loseWeaponsOnDeathInstructions, sizeof(loseWeaponsOnDeathInstructions));
}