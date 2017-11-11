//------------------------------------------------------------------------------------------------
// File: StartingPoint.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Send and receives OpenCV Mat Images in a UDP message commpressed as Jpeg images 
//------------------------------------------------------------------------------------------------
#include "StartingPoint.h"
#include "Direction.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;


StartingPoint::StartingPoint(int initialPositionX, int initialPositionY) {
	PositionX = initialPositionX;
	PositionY = initialPositionY;
}

void StartingPoint::reset(int robotEWSNDirection, int robotPositionX, int robotPositionY) {

	switch (robotEWSNDirection) {
	case EAST:
		resetEast(robotPositionX, robotPositionY);
		break;
	case WEST:
		resetWest(robotPositionX, robotPositionY);
		break;
	case SOUTH:
		resetSouth(robotPositionX, robotPositionY);
		break;
	case NORTH:
		resetNorth(robotPositionX, robotPositionY);
		break;
	default:
		printf("reset() - This is not supported direction. (%d)\n", robotEWSNDirection);
		break;
	}
}

void StartingPoint::resetEast(int robotPositionX, int robotPositionY) {
	// TODO: Caculate reset position...
}

void StartingPoint::resetWest(int robotPositionX, int robotPositionY) {
	// TODO: Caculate reset position...
}

void StartingPoint::resetSouth(int robotPositionX, int robotPositionY) {
	// TODO: Caculate reset position...
}

void StartingPoint::resetNorth(int robotPositionX, int robotPositionY) {
	// TODO: Caculate reset position...
}

bool StartingPoint::isStartingPoint(int robotPositionX, int robotPositionY) {
	if ((PositionX == robotPositionX) && (PositionY == robotPositionY)) {
		printf("isStartingPoint() - This is staring point.\n");
		return true;
	}

	return false;
}

//-----------------------------------------------------------------
// END WallFinder
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
