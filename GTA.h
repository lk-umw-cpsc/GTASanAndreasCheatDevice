#pragma once

struct Vector3d {
	float x, y, z;
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
	unsigned int* pTouch;
	float* health;
	unsigned int* color;
	unsigned int* lock;
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

#define PEDESTRIAN_HEALTH_OFFSET					0x540
#define PEDESTRIAN_ARMOR_OFFSET						0x548
#define PEDESTRIAN_OXYGEN_OFFSET					0x550

#define VEHICLE_HEALTH_OFFSET						0x4C0
#define VEHICLE_DRIVER_POINTER_OFFSET				0x460
#define VEHICLE_COLOR_OFFSET						0x434
#define VEHICLE_LOCK_OFFSET							0x4F8
#define VEHICLE_LOCK_STATE_LOCKED					2
#define VEHICLE_PASSENGER_OFFSET					0x460

#define WANTED_LEVEL_STRUCT_POINTER					0x00B7CD9C
#define WANTED_LEVEL_HEAT_OFFSET					0x0
#define WANTED_LEVEL_STARS_OFFSET					0x2C

#define WEAPON_AMMO_DECREMENT1_ADDRESS				0x7428B0
#define WEAPON_AMMO_DECREMENT2_ADDRESS				0x7428E6
#define WEAPON_AMMO_DECREMENT3_ADDRESS				0x73FA86 
#define WEAPON_AMMO_DECREMENT4_ADDRESS				0x73FAAF
#define WEAPON_AMMO_DECREMENT_SIZE					3

#define VEHICLE_LOCK_CHECK_ADDRESS					0x6D1E59
#define VEHICLE_LOCK_CHECK_SIZE						2

#define CLASS_PLAYER								0x0086d168

#define CLASS_CAR									0x00871120
#define CLASS_MOTORCYCLE							0x00871360
#define CLASS_PLANE									0x00871948
#define CLASS_HELICOPTER							0x00871680
#define CLASS_BICYCLE								0x00871528

const unsigned int *const pPlayerPedBaseAddress = 		(const unsigned int*)PLAYER_PEDESTRIAN_STRUCT_POINTER;
const unsigned int *const pPlayerVehicleBaseAddress =	(const unsigned int*)PLAYER_VEHICLE_STRUCT_POINTER;
const unsigned int *const pWantedLevelBaseAddress = 	(const unsigned int*)WANTED_LEVEL_STRUCT_POINTER;