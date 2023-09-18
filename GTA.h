#pragma once

typedef struct Vector3d {
	float x, y, z;
};

typedef struct Pedestrian {
	unsigned int baseAddress;
	unsigned int* pMultiVectorBaseAddress;
	unsigned int multiVectorBaseAddress;
	Vector3d* left;
	Vector3d* forward;
	Vector3d* up;
	Vector3d* position;
	Vector3d* velocity;
	Vector3d* rotationalVelocity;
	float* health;
	float* armor;
	float* oxygen;
};

typedef struct Vehicle {
	unsigned int baseAddress;
	unsigned int* pMultiVectorBaseAddress;
	unsigned int multiVectorBaseAddress;
	Vector3d* left;
	Vector3d* forward;
	Vector3d* up;
	Vector3d* position;
	Vector3d* velocity;
	Vector3d* rotationalVelocity;
	float* health;
};

typedef struct WantedLevel {
	unsigned int baseAddress;
	int* stars;
	int* heat;
};

void hookPedestrian(unsigned int baseAddress, Pedestrian* pedOut);
void hookVehicle(unsigned int baseAddress, Vehicle* vehicleOut);
void hookWantedLevel(unsigned int baseAddress, WantedLevel* wlOut);

#define PLAYER_PEDESTRIAN_STRUCT_POINTER			0x00B6F5F0
#define PLAYER_VEHICLE_STRUCT_POINTER				0x00BA18FC

#define MULTI_VECTOR_STRUCT_POINTER_OFFSET			0x14
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

#define WANTED_LEVEL_STRUCT_POINTER					0x00B7CD9C
#define WANTED_LEVEL_HEAT_OFFSET					0x0
#define WANTED_LEVEL_STARS_OFFSET					0x2C

const unsigned int* pPlayerPedBaseAddress = (unsigned int*)PLAYER_PEDESTRIAN_STRUCT_POINTER;
const unsigned int* pPlayerVehicleBaseAddress = (unsigned int*)PLAYER_VEHICLE_STRUCT_POINTER;
const unsigned int* pWantedLevelBaseAddress = (unsigned int*)WANTED_LEVEL_STRUCT_POINTER;