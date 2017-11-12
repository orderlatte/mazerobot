/*
 * FloorFinder.h
 *
 *  Created on: November 7, 2017
 *      Author: lg
 */

#ifndef FloorFinderH
#define FloorFinderH

#include "RobotVisionManager.h"

class FloorFinder {

public:
	bool RedDot;
//	bool StartingPoint;
	bool Goal;
	bool RedDotRecognize;
	int Sign_type;
	int Sign_position;
	

public:
	FloorFinder();
	void printDebug();
	void init();
};


#endif /* FloorFinderH */
