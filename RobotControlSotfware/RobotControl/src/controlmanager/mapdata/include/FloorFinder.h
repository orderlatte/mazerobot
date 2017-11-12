/*
 * FloorFinder.h
 *
 *  Created on: November 7, 2017
 *      Author: lg
 */

#ifndef FloorFinderH
#define FloorFinderH

#include "RobotVisionManager.h"

#define RED_DOT_MAX 8

typedef struct
{
	int positionX;
	int positionY;
} T_red_dot_position;

class FloorFinder {

public:
	bool RedDot;
//	bool StartingPoint;
	bool Goal;
	bool RedDotRecognize;
	int Sign_type;
	int Sign_position;
	T_red_dot_position RedDots[RED_DOT_MAX];
	int RedDotCount;
	

public:
	FloorFinder();
	void printDebug();
	void init();
	void setRedDotPosition(int positionX, int positionY);
	bool isAlreadyFoundedRedDot(int positionX, int positionY);
};


#endif /* FloorFinderH */
