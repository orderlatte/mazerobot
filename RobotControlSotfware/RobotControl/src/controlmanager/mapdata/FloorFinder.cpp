//------------------------------------------------------------------------------------------------
// File: WallFinder.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Send and receives OpenCV Mat Images in a UDP message commpressed as Jpeg images 
//------------------------------------------------------------------------------------------------
#include "FloorFinder.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>


using namespace std;


FloorFinder::FloorFinder() {
	init();
}

void FloorFinder::init() {
	RedDot = false;
//	StartingPoint = false;
	Goal = false;
	RedDotRecognize = false;
	Sign_position = 0;
	Sign_type = 0;
	memset(RedDots, 0x00, sizeof(RedDots));
	RedDotCount = 0;
}

void FloorFinder::printDebug() {
	printf("RedDot: %d\n", RedDot);
//	printf("StartingPoint: %d\n", StartingPoint);
	printf("Goal: %d\n", Goal);
}

void FloorFinder::setRedDotPosition(int positionX, int positionY) {
	RedDots[RedDotCount].positionX = positionX;
	RedDots[RedDotCount].positionY = positionY;
	RedDotCount++;
}

bool FloorFinder::isAlreadyFoundedRedDot(int positionX, int positionY) {
	int index;
	for (index = 0; index < RedDotCount; index++) {
		if ((positionX == RedDots[RedDotCount].positionX) && (positionY == RedDots[RedDotCount].positionY)) {
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------
// END WallFinder
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
