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
}

void FloorFinder::printDebug() {
	printf("RedDot: %d\n", RedDot);
//	printf("StartingPoint: %d\n", StartingPoint);
	printf("Goal: %d\n", Goal);
}

//-----------------------------------------------------------------
// END WallFinder
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
