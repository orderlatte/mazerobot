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
//	PositionX = -1;
//	PositionY = -1;

	int relativePositionX = PositionX - robotPositionX;
	int relativePositionY = PositionY - robotPositionY;

	switch (robotEWSNDirection) {
	case EAST:
		PositionX = PositionX + relativePositionY;
		PositionY = PositionY - relativePositionX;
		break;
	case WEST:
		PositionX = PositionX - relativePositionY;
		PositionY = PositionY + relativePositionX;
		break;
	case SOUTH:
		PositionX = PositionX - relativePositionX;
		PositionY = PositionY - relativePositionY;
		break;
	case NORTH:
		PositionX = PositionX + relativePositionX;
		PositionY = PositionY + relativePositionY;
		break;
	default:
		printf("reset() - This is not supported direction. (%d)\n", robotEWSNDirection);
		break;
	}
}

bool StartingPoint::isStartingPoint(int robotPositionX, int robotPositionY) {
	if ((PositionX == robotPositionX) && (PositionY == robotPositionY)) {
		printf("isStartingPoint() - This is staring point.\n");
		return true;
	}

	return false;
}

void StartingPoint::setStartingPosition(int robotPositionX, int robotPositionY) {
	PositionX = robotPositionX;
	PositionY = robotPositionY;
}

//-----------------------------------------------------------------
// END WallFinder
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
