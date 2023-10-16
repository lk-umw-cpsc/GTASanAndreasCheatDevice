#pragma once

#include <Windows.h>

struct Vector3d {
	float x, y, z;
};

struct VehicleName {
	char str[14];
};

struct WheelState {
	byte popped[4];
};

struct Pedestrian {
	unsigned int baseAddress;
	unsigned int objectClass;
	unsigned int* pMultiVectorBaseAddress;
	unsigned int multiVectorBaseAddress;
	Vector3d* left;
	Vector3d* forward;
	Vector3d* up;
	Vector3d* position;
	Vector3d* velocity;
	Vector3d* rotationalVelocity;
	Vector3d* impulse;
	Vector3d* rotationalImpulse;
	unsigned int* pTouch;
	float* health;
	float* armor;
	float* oxygen;
};

struct Vehicle {
	unsigned int baseAddress;
	unsigned int objectClass;
	unsigned int* pMultiVectorBaseAddress;
	unsigned int multiVectorBaseAddress;
	Vector3d* left;
	Vector3d* forward;
	Vector3d* up;
	Vector3d* position;
	Vector3d* velocity;
	Vector3d* rotationalVelocity;
	Vector3d* impulse;
	Vector3d* rotationalImpulse;
	unsigned short* id;
	float* mass;
	unsigned int* pTouch;
	float* health;
	unsigned int* lock;
	float* tireRotation;
	byte* nosCount;
	float* nosAmount;
	WheelState* wheelState;
	byte* color1;
	byte* color2;
};

struct WantedLevel {
	unsigned int baseAddress;
	int* stars;
	int* heat;
};

void hookPedestrian(unsigned int baseAddress, Pedestrian* pedOut);
void hookPedestrianMultiVector(unsigned int baseAddress, Pedestrian* pedOut);
void hookVehicle(unsigned int baseAddress, Vehicle* vehicleOut);
void hookVehicleMultiVector(unsigned int baseAddress, Vehicle* vehicleOut);
void hookWantedLevel(unsigned int baseAddress, WantedLevel* wlOut);

#define EXE_OFFSET 0x400000 // offset of items from the base address of sanandreas.exe

#define PLAYER_PEDESTRIAN_STRUCT_POINTER			0x00B6F5F0
#define PLAYER_VEHICLE_STRUCT_POINTER				0x00BA18FC

#define MULTI_VECTOR_STRUCT_POINTER_OFFSET			0x14
#define ID_OFFSET									0x22
#define TOUCH_POINTER_OFFSET						0xDC
// Found within above struct
#define LEFT_VECTOR_STRUCT_OFFSET					0x0
#define FORWARD_VECTOR_STRUCT_OFFSET				0x10
#define UP_VECTOR_STRUCT_OFFSET						0x20
#define POSITION_VECTOR_STRUCT_OFFSET				0x30

// Found within ped/vehicle struct directly
#define VELOCITY_VECTOR_STRUCT_OFFSET				0x44
#define ROTATIONAL_VELOCITY_VECTOR_STRUCT_OFFSET	0x50
#define IMPULSE_VECTOR_OFFSET						0x5C
#define ROTATIONAL_IMPULSE_VECTOR_OFFSET			0x68

#define PEDESTRIAN_HEALTH_OFFSET					0x540
#define PEDESTRIAN_ARMOR_OFFSET						0x548
#define PEDESTRIAN_OXYGEN_OFFSET					0x550

#define VEHICLE_ID_OFFSET							0x22
#define VEHICLE_MASS_OFFSET							0x8C
#define VEHICLE_HEALTH_OFFSET						0x4C0
#define VEHICLE_DRIVER_POINTER_OFFSET				0x460
#define VEHICLE_COLOR1_OFFSET						0x434
#define VEHICLE_COLOR2_OFFSET						0x435
#define VEHICLE_LOCK_OFFSET							0x4F8
#define VEHICLE_LOCK_STATE_LOCKED					2
#define VEHICLE_LOCK_STATE_UNLOCKED					1
#define VEHICLE_WHEEL_STATE_OFFSET					0x5A5
#define VEHICLE_WHEEL_SIZE_OFFSET					0x458
#define VEHICLE_NOS_COUNT_OFFSET					0x48A
#define VEHICLE_NOS_AMOUNT_OFFSET					0x8A4

// 0x898 tire?
// 0x8cc bounce factor?
// 0x24 ??? pointer to something that changes as car's color changes

#define PLANE_DAMAGE_OFFSET							0x5B4 //DWORD

#define VEHICLE_PASSENGER_OFFSET					0x460
#define VEHICLE_TIRE_ROTATION_OFFSET				0x898

#define WANTED_LEVEL_STRUCT_POINTER					0x00B7CD9C
#define WANTED_LEVEL_HEAT_OFFSET					0x0
#define WANTED_LEVEL_STARS_OFFSET					0x2C
#define NO_CRIMES_FLAG_ADDRESS						0x00969171
#define EXPLORATION_CRIME_INSTRUCTION_ADDRESS		0x5624da

#define WEAPON_AMMO_DECREMENT1_ADDRESS				0x7428B0
#define WEAPON_AMMO_DECREMENT2_ADDRESS				0x7428E6
#define WEAPON_AMMO_DECREMENT3_ADDRESS				0x73FA86 
#define WEAPON_AMMO_DECREMENT4_ADDRESS				0x73FAAF
#define WEAPON_AMMO_DECREMENT_SIZE					3

#define VEHICLE_LOCK_CHECK_ADDRESS					0x6D1E59
#define VEHICLE_LOCK_CHECK_SIZE						2

#define PLANE_DAMAGE_CHECK_INSTRUCTION_ADDRESS		0x6c2474
#define PLANE_DAMAGE_CHECK_INSTRUCTION_SIZE			2

#define CLASS_PLAYER								0x0086d168

#define CLASS_CAR									0x00871120
#define CLASS_MOTORCYCLE							0x00871360
#define CLASS_PLANE									0x00871948
#define CLASS_HELICOPTER							0x00871680
#define CLASS_BICYCLE								0x00871528
#define CLASS_BOAT									0x008721A0

#define DISPLAY_MESSAGE_FUNCTION_ADDRESS			0x00588BE0

#define ENTITY_INFO_TABLE_BASE_ADDRESS				0xA9B0C8

extern const unsigned int* const pPlayerPedBaseAddress;
extern const unsigned int* const pPlayerVehicleBaseAddress;
extern const unsigned int* const pWantedLevelBaseAddress;
extern byte* const pNoCrimesFlag;

extern void(*displayMessage)(const char*, unsigned int, unsigned int, unsigned int);
extern void (*spawnCar)(unsigned short);
extern unsigned int* entityInfo;

/*
"Crime" instructions

562e2a 9090
562e31 9090
562275 9090
562266 9090

5624ad 909090909090
5624da 909090909090

Prevent function call
562470 c2 04 00
562c90 c3
562120 c20c00

*/

//7fde3b ??? D3D call