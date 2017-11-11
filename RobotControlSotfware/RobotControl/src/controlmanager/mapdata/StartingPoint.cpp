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
		resetPosition(robotPosionY, -robotPositonX);
		break;
	case WEST:
		resetPosition(-robotPositionY, robotPositionX);
		break;
	case SOUTH:
		resetPosition(-robotPositionX, -robotPositionY);
		break;
	case NORTH:
		resetPosition(robotPositionX, robotPositionY);
		break;
	default:
		printf("reset() - This is not supported direction. (%d)\n", robotEWSNDirection);
		break;
	}
}

void StartingPoint::resetPosition(int robotPositionX, int robotPositionY) {
	// TODO: Caculate reset position...
	PositionX = 5 + robotPositionX;
	PositionY = 5 + robotPositionY;
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
