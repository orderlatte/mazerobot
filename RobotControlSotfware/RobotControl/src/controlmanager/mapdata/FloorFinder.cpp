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

static T_red_dots RedDots[RED_DOT_MAX] = {0, };
static int RedDotCount = 0;


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
}

void FloorFinder::reset() {
	init();
	memset(RedDots, 0x00, sizeof(RedDots));
	RedDotCount = 0;
}

void FloorFinder::printDebug() {
	printf("RedDot: %d\n", RedDot);
//	printf("StartingPoint: %d\n", StartingPoint);
	printf("Goal: %d\n", Goal);
}

void FloorFinder::setRedDotPosition(int positionX, int positionY) {
	printf("setRedDotPosition(%d, %d)\n", positionX, positionY);
	RedDots[RedDotCount].positionX = positionX;
	RedDots[RedDotCount].positionY = positionY;
	RedDotCount++;
}

void FloorFinder::setRedDotSign(int index, int type, int wallPosition) {
	RedDots[index].type = type;
	RedDots[index].wallPosition = wallPosition;
}

int FloorFinder::getRedDotSign(int positionX, int positionY, int *type, int *wallPosition) {
	int index;
	for (index = 0; index < RedDotCount; index++) {
		if ((positionX == RedDots[index].positionX) && (positionY == RedDots[index].positionY)) {
			*type = RedDots[index].type;
			*wallPosition = RedDots[index].wallPosition;
			return index;
		}
	}

	return -1;
}

bool FloorFinder::isAlreadyFoundedRedDot(int positionX, int positionY) {
	int index;
	for (index = 0; index < RedDotCount; index++) {
		if ((positionX == RedDots[index].positionX) && (positionY == RedDots[index].positionY)) {
			printf("(%d, %d) has already red dot!\n", positionX, positionY);
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
