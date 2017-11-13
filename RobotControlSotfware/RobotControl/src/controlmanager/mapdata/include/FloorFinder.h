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
	int type;
	int wallPosition;
} T_red_dots;

typedef struct
{
	int positionX;
	int positionY;
} T_goal;

class FloorFinder {

public:
	bool RedDot;
//	bool StartingPoint;
//	bool Goal;
	bool RedDotRecognize;
	int Sign_type;
	int Sign_position;

public:
	FloorFinder();
	void printDebug();
	void init();
	void reset();
	void setRedDotPosition(int positionX, int positionY);
	void setRedDotSign(int index, int type, int wallPosition);
	bool setRedDotSign(int positionX, int positionY);
	int getRedDotSign(int positionX, int positionY, int *type, int *wallPosition);
	bool isAlreadyFoundedRedDot(int positionX, int positionY);
	void setGoalPosition(int positionX, int positionY);
	bool isAlreadyFoundedGoal(int positionX, int positionY);
};


#endif /* FloorFinderH */
