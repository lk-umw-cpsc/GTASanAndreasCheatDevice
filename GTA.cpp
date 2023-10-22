#include "GTA.h"

void hookWantedLevel(DWORD baseAddress, WantedLevel* wlOut) {
	wlOut->baseAddress = baseAddress;
	wlOut->heat = (int*)(baseAddress + WANTED_LEVEL_HEAT_OFFSET);
	wlOut->stars = (int*)(baseAddress + WANTED_LEVEL_STARS_OFFSET);
}

void hookPedestrian(DWORD baseAddress, Pedestrian* pedOut) {
	pedOut->baseAddress = baseAddress;
	pedOut->pMultiVectorBaseAddress = (DWORD*)(baseAddress + MULTI_VECTOR_STRUCT_POINTER_OFFSET);
	
	unsigned int multiVectorBaseAddress = *pedOut->pMultiVectorBaseAddress;
	hookPedestrianMultiVector(multiVectorBaseAddress, pedOut);

	pedOut->velocity =				(Vector3d*)(baseAddress + VELOCITY_VECTOR_STRUCT_OFFSET);
	pedOut->rotationalVelocity =	(Vector3d*)(baseAddress + ROTATIONAL_VELOCITY_VECTOR_STRUCT_OFFSET);
	pedOut->impulse =				(Vector3d*)(baseAddress + IMPULSE_VECTOR_OFFSET);
	pedOut->rotationalImpulse =		(Vector3d*)(baseAddress + ROTATIONAL_IMPULSE_VECTOR_OFFSET);
	pedOut->pTouch = (DWORD*)(baseAddress + TOUCH_POINTER_OFFSET);
	pedOut->health = (float*)(baseAddress + PEDESTRIAN_HEALTH_OFFSET);
	pedOut->armor = (float*)(baseAddress + PEDESTRIAN_ARMOR_OFFSET);
	pedOut->oxygen = (float*)(baseAddress + PEDESTRIAN_OXYGEN_OFFSET);
}

void hookPedestrianMultiVector(DWORD baseAddress, Pedestrian* pedOut) {
	pedOut->multiVectorBaseAddress = baseAddress;
	pedOut->left = (Vector3d*)(baseAddress + LEFT_VECTOR_STRUCT_OFFSET);
	pedOut->forward = (Vector3d*)(baseAddress + FORWARD_VECTOR_STRUCT_OFFSET);
	pedOut->up = (Vector3d*)(baseAddress + UP_VECTOR_STRUCT_OFFSET);
	pedOut->position = (Vector3d*)(baseAddress + POSITION_VECTOR_STRUCT_OFFSET);
}

void hookVehicle(DWORD baseAddress, Vehicle* vehicleOut) {
	vehicleOut->baseAddress = baseAddress;
	vehicleOut->objectClass = *reinterpret_cast<unsigned int*>(baseAddress);
	vehicleOut->pMultiVectorBaseAddress = (DWORD*)(baseAddress + MULTI_VECTOR_STRUCT_POINTER_OFFSET);

	unsigned int multiVectorBaseAddress = *vehicleOut->pMultiVectorBaseAddress;
	hookVehicleMultiVector(multiVectorBaseAddress, vehicleOut);

	vehicleOut->velocity = (Vector3d*)(baseAddress + VELOCITY_VECTOR_STRUCT_OFFSET);
	vehicleOut->rotationalVelocity = (Vector3d*)(baseAddress + ROTATIONAL_VELOCITY_VECTOR_STRUCT_OFFSET);
	vehicleOut->impulse = (Vector3d*)(baseAddress + IMPULSE_VECTOR_OFFSET);
	vehicleOut->rotationalImpulse = (Vector3d*)(baseAddress + ROTATIONAL_IMPULSE_VECTOR_OFFSET);
	vehicleOut->pTouch = (DWORD*)(baseAddress + TOUCH_POINTER_OFFSET);
	vehicleOut->health = (float*)(baseAddress + VEHICLE_HEALTH_OFFSET);
	vehicleOut->color1 = reinterpret_cast<byte*>(baseAddress + VEHICLE_COLOR1_OFFSET);
	vehicleOut->color2 = reinterpret_cast<byte*>(baseAddress + VEHICLE_COLOR2_OFFSET);
	vehicleOut->lock = reinterpret_cast<DWORD*>(baseAddress + VEHICLE_LOCK_OFFSET);
	vehicleOut->id = reinterpret_cast<WORD*>(baseAddress + VEHICLE_ID_OFFSET);
	vehicleOut->tireRotation = reinterpret_cast<float*>(baseAddress + VEHICLE_TIRE_ROTATION_OFFSET);
	vehicleOut->mass = (float*)(baseAddress + VEHICLE_MASS_OFFSET);
	vehicleOut->wheelState = (WheelState*)(baseAddress + VEHICLE_WHEEL_STATE_OFFSET);
	vehicleOut->mods = (byte*)(baseAddress + VEHICLE_MOD_OFFSET);
	vehicleOut->nosCount = (byte*)(baseAddress + VEHICLE_NOS_COUNT_OFFSET);
	vehicleOut->nosAmount = (float*)(baseAddress + VEHICLE_NOS_AMOUNT_OFFSET);
}

void hookVehicleMultiVector(DWORD baseAddress, Vehicle* vehicleOut) {
	vehicleOut->multiVectorBaseAddress = baseAddress;
	vehicleOut->left = (Vector3d*)(baseAddress + LEFT_VECTOR_STRUCT_OFFSET);
	vehicleOut->forward = (Vector3d*)(baseAddress + FORWARD_VECTOR_STRUCT_OFFSET);
	vehicleOut->up = (Vector3d*)(baseAddress + UP_VECTOR_STRUCT_OFFSET);
	vehicleOut->position = (Vector3d*)(baseAddress + POSITION_VECTOR_STRUCT_OFFSET);
}

const DWORD* const pPlayerPedBaseAddress = (DWORD*)PLAYER_PEDESTRIAN_STRUCT_POINTER;
const DWORD* const pPlayerVehicleBaseAddress = (DWORD*)PLAYER_VEHICLE_STRUCT_POINTER;
const DWORD* const pWantedLevelBaseAddress = (DWORD*)WANTED_LEVEL_STRUCT_POINTER;
EntityTable** ppPedestrianTable = (EntityTable**)PEDESTRIAN_TABLE_POINTER_ADDRESS;
EntityTable** ppVehicleTable = (EntityTable**)VEHICLE_TABLE_POINTER_ADDRESS;
byte* const pNoCrimesFlag = (byte*)NO_CRIMES_FLAG_ADDRESS;
float* gravity = (float*)(GRAVITY_ADDRESS);

void(*displayMessage)(const char*, DWORD, DWORD, DWORD) = (void(*)(const char*, DWORD, DWORD, DWORD))DISPLAY_MESSAGE_FUNCTION_ADDRESS;
void (*spawnCar)(WORD) = (void(*)(WORD))0x43A0B0;
DWORD* entityInfo = reinterpret_cast<DWORD*>(ENTITY_INFO_TABLE_BASE_ADDRESS);

// Spawn explosion:
// ((void(*)(DWORD, WORD, float, float, float, BYTE, BYTE, BYTE))0x737c80)(pplayer, 0x13, *(float*)(vec + 0x30), *(float*)(vec + 0x34), *(float*)(vec + 0x38), 0, 0, 0);

BYTE* gamePaused = (BYTE*)GAME_PAUSED_FLAG_ADDRESS;
BYTE* inputDisabled = (BYTE*)INPUT_DISABLED_FLAG_ADDRESS;