//------------------------------------------------------------------------------------------------
// File: WallFinder.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Send and receives OpenCV Mat Images in a UDP message commpressed as Jpeg images 
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "WallFinder.h"


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
	if (sensorData->sonar_distance < 10) { // Less than 10cm
		printf("recognizeWall() Front wall is existed\n");
		BlockedFrontWall = true;
	}

	if (sensorData->laser_left_distance < 100) {  // Less than
		printf("recognizeWall() Left wall is existed\n");
		BlockedLeftWall = true;
	}

	if (sensorData->laser_right_distance < 100) {  // Less than
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


//-----------------------------------------------------------------
// END WallFinder
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
