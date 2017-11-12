/*
 * WallFinder.h
 *
 *  Created on: November 7, 2017
 *      Author: lg
 */

#ifndef WallFinderH
#define WallFinderH

#include "sensor_manager.h"

class WallFinder {

private:
	bool CheckedFrontWall;
	bool CheckedLeftWall;
	bool CheckedRightWall;
	bool CheckedBackWall;
	bool BlockedFrontWall;
	bool BlockedLeftWall;
	bool BlockedRightWall;
	bool BlockedBackWall;
//	int CurrentEWSNDirection;

public:
	WallFinder();
	void Init();
	void recognizeWall();
	bool getCheckedFrontWall();
	bool getCheckedLeftWall();
	bool getCheckedRightWall();
	bool getCheckedBackWall();
	bool getBlockedFrontWall();
	bool getBlockedLeftWall();
	bool getBlockedRightWall();
	bool getBlockedBackWall();
	unsigned char getCheckedWalls(int EWSNDirection);
	unsigned char getBlockedWalls(int EWSNDirection);

private:
	unsigned char getCheckedEastWalls();
	unsigned char getCheckedWestWalls();
	unsigned char getCheckedSouthWalls();
	unsigned char getCheckedNorthWalls();
	unsigned char getBlockedEastWalls();
	unsigned char getBlockedWestWalls();
	unsigned char getBlockedSouthWalls();
	unsigned char getBlockedNorthWalls();
	void printDebug();
};


#endif /* WallFinderH */
