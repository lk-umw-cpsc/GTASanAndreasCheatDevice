#include "GTA.h"

void hookWantedLevel(unsigned int baseAddress, WantedLevel* wlOut) {
	wlOut->baseAddress = baseAddress;
	wlOut->heat = (int*)(baseAddress + WANTED_LEVEL_HEAT_OFFSET);
	wlOut->stars = (int*)(baseAddress + WANTED_LEVEL_STARS_OFFSET);
}

void hookPedestrian(unsigned int baseAddress, Pedestrian* pedOut) {
	pedOut->baseAddress = baseAddress;
	pedOut->pMultiVectorBaseAddress = (unsigned int*)(baseAddress + MULTI_VECTOR_STRUCT_POINTER_OFFSET);
	
	unsigned int multiVectorBaseAddress = *pedOut->pMultiVectorBaseAddress;
	hookPedestrianMultiVector(multiVectorBaseAddress, pedOut);

	pedOut->velocity =				(Vector3d*)(baseAddress + VELOCITY_VECTOR_STRUCT_OFFSET);
	pedOut->rotationalVelocity =	(Vector3d*)(baseAddress + ROTATIONAL_VELOCITY_VECTOR_STRUCT_OFFSET);
	pedOut->impulse =				(Vector3d*)(baseAddress + IMPULSE_VECTOR_OFFSET);
	pedOut->rotationalImpulse =		(Vector3d*)(baseAddress + ROTATIONAL_IMPULSE_VECTOR_OFFSET);
	pedOut->pTouch = (unsigned int*)(baseAddress + TOUCH_POINTER_OFFSET);
	pedOut->health = (float*)(baseAddress + PEDESTRIAN_HEALTH_OFFSET);
	pedOut->armor = (float*)(baseAddress + PEDESTRIAN_ARMOR_OFFSET);
	pedOut->oxygen = (float*)(baseAddress + PEDESTRIAN_OXYGEN_OFFSET);
}

void hookPedestrianMultiVector(unsigned int baseAddress, Pedestrian* pedOut) {
	pedOut->multiVectorBaseAddress = baseAddress;
	pedOut->left = (Vector3d*)(baseAddress + LEFT_VECTOR_STRUCT_OFFSET);
	pedOut->forward = (Vector3d*)(baseAddress + FORWARD_VECTOR_STRUCT_OFFSET);
	pedOut->up = (Vector3d*)(baseAddress + UP_VECTOR_STRUCT_OFFSET);
	pedOut->position = (Vector3d*)(baseAddress + POSITION_VECTOR_STRUCT_OFFSET);
}

void hookVehicle(unsigned int baseAddress, Vehicle* vehicleOut) {
	vehicleOut->baseAddress = baseAddress;
	vehicleOut->objectClass = *reinterpret_cast<unsigned int*>(baseAddress);
	vehicleOut->pMultiVectorBaseAddress = (unsigned int*)(baseAddress + MULTI_VECTOR_STRUCT_POINTER_OFFSET);

	unsigned int multiVectorBaseAddress = *vehicleOut->pMultiVectorBaseAddress;
	hookVehicleMultiVector(multiVectorBaseAddress, vehicleOut);

	vehicleOut->velocity = (Vector3d*)(baseAddress + VELOCITY_VECTOR_STRUCT_OFFSET);
	vehicleOut->rotationalVelocity = (Vector3d*)(baseAddress + ROTATIONAL_VELOCITY_VECTOR_STRUCT_OFFSET);
	vehicleOut->impulse = (Vector3d*)(baseAddress + IMPULSE_VECTOR_OFFSET);
	vehicleOut->rotationalImpulse = (Vector3d*)(baseAddress + ROTATIONAL_IMPULSE_VECTOR_OFFSET);
	vehicleOut->pTouch = (unsigned int*)(baseAddress + TOUCH_POINTER_OFFSET);
	vehicleOut->health = (float*)(baseAddress + VEHICLE_HEALTH_OFFSET);
	vehicleOut->color1 = reinterpret_cast<byte*>(baseAddress + VEHICLE_COLOR1_OFFSET);
	vehicleOut->color2 = reinterpret_cast<byte*>(baseAddress + VEHICLE_COLOR2_OFFSET);
	vehicleOut->lock = reinterpret_cast<unsigned int*>(baseAddress + VEHICLE_LOCK_OFFSET);
	vehicleOut->id = reinterpret_cast<unsigned short*>(baseAddress + VEHICLE_ID_OFFSET);
	vehicleOut->tireRotation = reinterpret_cast<float*>(baseAddress + VEHICLE_TIRE_ROTATION_OFFSET);
	vehicleOut->mass = (float*)(baseAddress + VEHICLE_MASS_OFFSET);
	vehicleOut->wheelState = (WheelState*)(baseAddress + VEHICLE_WHEEL_STATE_OFFSET);
}

void hookVehicleMultiVector(unsigned int baseAddress, Vehicle* vehicleOut) {
	vehicleOut->multiVectorBaseAddress = baseAddress;
	vehicleOut->left = (Vector3d*)(baseAddress + LEFT_VECTOR_STRUCT_OFFSET);
	vehicleOut->forward = (Vector3d*)(baseAddress + FORWARD_VECTOR_STRUCT_OFFSET);
	vehicleOut->up = (Vector3d*)(baseAddress + UP_VECTOR_STRUCT_OFFSET);
	vehicleOut->position = (Vector3d*)(baseAddress + POSITION_VECTOR_STRUCT_OFFSET);
}

const unsigned int* const pPlayerPedBaseAddress = (unsigned int*)PLAYER_PEDESTRIAN_STRUCT_POINTER;
const unsigned int* const pPlayerVehicleBaseAddress = (unsigned int*)PLAYER_VEHICLE_STRUCT_POINTER;
const unsigned int* const pWantedLevelBaseAddress = (unsigned int*)WANTED_LEVEL_STRUCT_POINTER;
byte* const pNoCrimesFlag = (byte*)NO_CRIMES_FLAG_ADDRESS;

void(*displayMessage)(const char*, unsigned int, unsigned int, unsigned int) = (void(*)(const char*, unsigned int, unsigned int, unsigned int))DISPLAY_MESSAGE_FUNCTION_ADDRESS;
void (*spawnCar)(unsigned short) = (void(*)(unsigned short))0x43A0B0;
unsigned int* entityInfo = reinterpret_cast<unsigned int*>(ENTITY_INFO_TABLE_BASE_ADDRESS);
