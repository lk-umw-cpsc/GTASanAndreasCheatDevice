#pragma once

#include <Windows.h>

typedef void(__thiscall* voidObjectFunctionDWORDDWORD)(DWORD __this, DWORD, DWORD);
typedef void(__thiscall* voidObjectFunction)(DWORD __this);

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
	DWORD baseAddress;
	DWORD objectClass;
	DWORD* pMultiVectorBaseAddress;
	DWORD multiVectorBaseAddress;
	Vector3d* left;
	Vector3d* forward;
	Vector3d* up;
	Vector3d* position;
	Vector3d* velocity;
	Vector3d* rotationalVelocity;
	Vector3d* impulse;
	Vector3d* rotationalImpulse;
	DWORD* pTouch;
	DWORD* pTargetBaseAddress;
	float* health;
	float* armor;
};

struct Vehicle {
	DWORD baseAddress;
	DWORD objectClass;
	DWORD* pMultiVectorBaseAddress;
	DWORD multiVectorBaseAddress;
	Vector3d* left;
	Vector3d* forward;
	Vector3d* up;
	Vector3d* position;
	Vector3d* velocity;
	Vector3d* rotationalVelocity;
	Vector3d* impulse;
	Vector3d* rotationalImpulse;
	WORD* id;
	float* mass;
	DWORD* pTouch;
	float* health;
	DWORD* lock;
	float* tireRotation;
	byte* nosCount;
	float* nosAmount;
	WheelState* wheelState;
	byte* color1;
	byte* color2;
	byte* mods;
	byte* onGroundFlag;
};

struct WantedLevel {
	DWORD baseAddress;
	int* stars;
	int* heat;
};

struct EntityTable {
	DWORD arrayBaseAddress;
	byte* slotInUse;
	DWORD numSlots;
};

void hookPedestrian(DWORD baseAddress, Pedestrian* pedOut);
void hookPedestrianMultiVector(DWORD baseAddress, Pedestrian* pedOut);
void hookVehicle(DWORD baseAddress, Vehicle* vehicleOut);
void hookVehicleMultiVector(DWORD baseAddress, Vehicle* vehicleOut);
void hookWantedLevel(DWORD baseAddress, WantedLevel* wlOut);

#define EXE_OFFSET 0x400000 // offset of items from the base address of sanandreas.exe

#define PLAYER_PEDESTRIAN_STRUCT_POINTER			0x00B6F5F0
#define PLAYER_VEHICLE_STRUCT_POINTER				0x00BA18FC

#define PLAYER_OXYGEN_ADDRESS						0xB7CDE0

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
#define PEDESTRIAN_TARGET_POINTER_OFFSET			0x71C

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
#define VEHICLE_MOD_OFFSET							0x38E
#define VEHICLE_NOS_COUNT_OFFSET					0x48A // +0x38E NOS... OR 0x20... +0x43A 0x03F2
#define VEHICLE_NOS_AMOUNT_OFFSET					0x8A4
#define VEHICLE_ON_GROUND_FLAG_OFFSET				0xB9

#define NOS_ENABLED									0x08

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

#define PEDESTRIAN_TABLE_POINTER_ADDRESS			0xB74490
#define PEDESTRIAN_OBJECT_SIZE						0x7C4

#define VEHICLE_TABLE_POINTER_ADDRESS				0xB74494
#define VEHICLE_OBJECT_SIZE							0xA18

#define GRAVITY_ADDRESS								0x863984

#define POPUP_MENU_SHOWING_FLAG_ADDRESS				0x00BAA474 // byte

#define GAME_PAUSED_FLAG_ADDRESS					0xB7CB49
#define INPUT_DISABLED_FLAG_ADDRESS					0xB73566

#define TRIPPY_EFFECT_INSTRUCTION_ADDRESS			0x705138

#define BUNNY_HOP_MULTIPLIER_ADDRESS				0x00858C80
#define JUMP_OVER_BHMULTIPLIER_INSTRUCTION_ADDRESS	0x6C0439

#define LOSE_WEAPONS_ON_DEATH_INSTRUCTION_ADDRESS	0x5e6340


extern const DWORD* const pPlayerPedBaseAddress;
extern const DWORD* const pPlayerVehicleBaseAddress;
extern const DWORD* const pWantedLevelBaseAddress;
extern EntityTable** ppPedestrianTable;
extern EntityTable** ppVehicleTable;
extern byte* const pNoCrimesFlag;
extern float* gravity;
extern float* bunnyHopMultiplier;
extern float* playerOxygen;

extern void(*displayMessage)(const char*, DWORD, DWORD, DWORD);
extern void (*spawnExplosion)(DWORD cause, WORD id, float x, float y, float z, DWORD, DWORD, DWORD);
extern void (*spawnCar)(WORD);
extern voidObjectFunction freezePedestrian;
extern voidObjectFunction killPedestrian;
extern DWORD* entityInfo;
extern BYTE* gamePaused;
extern BYTE* inputDisabled;
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

// 0x6A8519 2 NOPs to enable flying cars
// 0x969179 set to 1 to enable free aim in car
// 0x679c41 2 NOPs enable jump multiplier
// 0x[0085862C] jump multiplier