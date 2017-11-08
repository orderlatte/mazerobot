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
	CheckedFrontWall = true;
	CheckedLeftWall = true;
	CheckedRightWall = true;
	CheckedBackWall = true;
	ConfirmedFrontWall = false;
	ConfirmedLeftWall = false;
	ConfirmedRightWall = false;
	ConfirmedBackWall = true;
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
		ConfirmedFrontWall = true;
	}

	if (sensorData->laser_left_distance < 10) {  // Less than
		ConfirmedLeftWall = true;
	}

	if (sensorData->laser_right_distance < 10) {  // Less than
		ConfirmedRightWall = true;
	}
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

bool WallFinder::getConfirmedFrontWall() {
	return ConfirmedFrontWall;
}

bool WallFinder::getConfirmedLeftWall() {
	return ConfirmedLeftWall;
}

bool WallFinder::getConfirmedRightWall() {
	return ConfirmedRightWall;
}

bool WallFinder::getConfirmedBackWall() {
	return ConfirmedBackWall;
}


//-----------------------------------------------------------------
// END WallFinder
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
