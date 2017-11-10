//------------------------------------------------------------------------------------------------
// File: WallFinder.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Send and receives OpenCV Mat Images in a UDP message commpressed as Jpeg images 
//------------------------------------------------------------------------------------------------
#include "WallFinder.h"
#include "Direction.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>


using namespace std;


WallFinder::WallFinder() {
	Init();
}

void WallFinder::Init() {
	CheckedFrontWall = true;
	CheckedLeftWall = true;
	CheckedRightWall = true;
	CheckedBackWall = true;
	BlockedFrontWall = false;
	BlockedLeftWall = false;
	BlockedRightWall = false;
	BlockedBackWall = false;
//	CurrentEWSNDirection = EWSNDirection;
}

//
//int getNextEWSNDirection(bool initialStarted) {
//	T_SensorData sensorData = get_sensor_data();
//	T_WallData wallData;
//
//	sendCurrentRobotStatus(&sensorData, &wallData, initialStarted);
//
//	// TODO: Below code will be replaced with Maze algorithm
//	return getTestNextDirection(&wallData);
//}

void WallFinder::recognizeWall(T_SensorData* sensorData) {
	if (sensorData->sonar_distance < 15 && sensorData->sonar_distance > 0) { // Less than 10cm
		printf("recognizeWall() Front wall is existed\n");
		BlockedFrontWall = true;
	}

	if (sensorData->laser_left_distance < 150 && sensorData->laser_left_distance > 0) {  // Less than
		printf("recognizeWall() Left wall is existed\n");
		BlockedLeftWall = true;
	}

	if (sensorData->laser_right_distance < 150 && sensorData->laser_right_distance > 0) {  // Less than
		printf("recognizeWall() Right wall is existed\n");
		BlockedRightWall = true;
	}

	printDebug();
}

void WallFinder::printDebug() {
	printf("CheckedFrontWall: %d\n", CheckedFrontWall);
	printf("CheckedLeftWall: %d\n", CheckedLeftWall);
	printf("CheckedRightWall: %d\n", CheckedRightWall);
	printf("CheckedBackWall: %d\n", CheckedBackWall);
	printf("BlockedFrontWall: %d\n", BlockedFrontWall);
	printf("BlockedLeftWall: %d\n", BlockedLeftWall);
	printf("BlockedRightWall: %d\n", BlockedRightWall);
	printf("BlockedBackWall: %d\n", BlockedBackWall);
}

bool WallFinder::getCheckedFrontWall() {
	return CheckedFrontWall;
}

bool WallFinder::getCheckedLeftWall() {
	return CheckedLeftWall;
}

bool WallFinder::getCheckedRightWall() {
	return CheckedRightWall;
}

bool WallFinder::getCheckedBackWall() {
	return CheckedBackWall;
}

bool WallFinder::getBlockedFrontWall() {
	printf("getBlockedFontWall() is called.(%d)\n", BlockedFrontWall);
	return BlockedFrontWall;
}

bool WallFinder::getBlockedLeftWall() {
	printf("getBlockedLeftWall() is called.(%d)\n", BlockedLeftWall);
	return BlockedLeftWall;
}

bool WallFinder::getBlockedRightWall() {
	printf("getBlockedRightWall() is called.(%d)\n", BlockedRightWall);
	return BlockedRightWall;
}

bool WallFinder::getBlockedBackWall() {
	printf("getBlockedBackWall() is called.(%d)\n", BlockedBackWall);
	return BlockedBackWall;
}

unsigned char WallFinder::getCheckedWalls(int EWSNDirection) {

	switch (EWSNDirection) {
	case EAST:
		return getCheckedEastWalls();

	case WEST:
		return getCheckedWestWalls();

	case SOUTH:
		return getCheckedSouthWalls();

	case NORTH:
		return getCheckedNorthWalls();

	default:
		printf("getCheckedWalls() - Error! Current EWNS is not supported direction.(%d)\n", EWSNDirection);
		return 0x00;
	}
}

unsigned char WallFinder::getCheckedEastWalls() {
	unsigned char walls = 0x00;

	if (CheckedFrontWall == true) {
		walls |= EAST;
	}

	if (CheckedBackWall == true) {
		walls |= WEST;
	}

	if (CheckedLeftWall == true) {
		walls |= NORTH;
	}

	if (CheckedRightWall == true) {
		walls |= SOUTH;
	}

	return walls;
}

unsigned char WallFinder::getCheckedWestWalls() {
	unsigned char walls = 0x00;

	if (CheckedFrontWall == true) {
		walls |= WEST;
	}

	if (CheckedBackWall == true) {
		walls |= EAST;
	}

	if (CheckedLeftWall == true) {
		walls |= SOUTH;
	}

	if (CheckedRightWall == true) {
		walls |= NORTH;
	}

	return walls;
}

unsigned char WallFinder::getCheckedSouthWalls() {
	unsigned char walls = 0x00;

	if (CheckedFrontWall == true) {
		walls |= SOUTH;
	}

	if (CheckedBackWall == true) {
		walls |= NORTH;
	}

	if (CheckedLeftWall == true) {
		walls |= EAST;
	}

	if (CheckedRightWall == true) {
		walls |= WEST;
	}

	return walls;
}

unsigned char WallFinder::getCheckedNorthWalls() {
	unsigned char walls = 0x00;

	if (CheckedFrontWall == true) {
		walls |= NORTH;
	}

	if (CheckedBackWall == true) {
		walls |= SOUTH;
	}

	if (CheckedLeftWall == true) {
		walls |= WEST;
	}

	if (CheckedRightWall == true) {
		walls |= EAST;
	}

	return walls;
}


unsigned char WallFinder::getBlockedWalls(int EWSNDirection) {

	switch (EWSNDirection) {
		case EAST:
			return getBlockedEastWalls();

		case WEST:
			return getBlockedWestWalls();

		case SOUTH:
			return getBlockedSouthWalls();

		case NORTH:
			return getBlockedNorthWalls();

		default:
			printf("getBlockedWalls() - Error! Current EWNS is not supported direction.(%d)\n", EWSNDirection);
			return 0x00;
		}
}

unsigned char WallFinder::getBlockedEastWalls() {
	unsigned char walls = 0x00;

	if (BlockedFrontWall == true) {
		walls |= EAST;
	}

	if (BlockedBackWall == true) {
		walls |= WEST;
	}

	if (BlockedLeftWall == true) {
		walls |= NORTH;
	}

	if (BlockedRightWall == true) {
		walls |= SOUTH;
	}

	return walls;
}

unsigned char WallFinder::getBlockedWestWalls() {
	unsigned char walls = 0x00;

	if (BlockedFrontWall == true) {
		walls |= WEST;
	}

	if (BlockedBackWall == true) {
		walls |= EAST;
	}

	if (BlockedLeftWall == true) {
		walls |= SOUTH;
	}

	if (BlockedRightWall == true) {
		walls |= NORTH;
	}

	return walls;
}

unsigned char WallFinder::getBlockedSouthWalls() {
	unsigned char walls = 0x00;

	if (BlockedFrontWall == true) {
		walls |= SOUTH;
	}

	if (BlockedBackWall == true) {
		walls |= NORTH;
	}

	if (BlockedLeftWall == true) {
		walls |= EAST;
	}

	if (BlockedRightWall == true) {
		walls |= WEST;
	}

	return walls;
}

unsigned char WallFinder::getBlockedNorthWalls() {
	unsigned char walls = 0x00;

	if (BlockedFrontWall == true) {
		walls |= NORTH;
	}

	if (BlockedBackWall == true) {
		walls |= SOUTH;
	}

	if (BlockedLeftWall == true) {
		walls |= WEST;
	}

	if (BlockedRightWall == true) {
		walls |= EAST;
	}

	return walls;
}


//-----------------------------------------------------------------
// END WallFinder
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
