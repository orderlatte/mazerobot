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

public:
	WallFinder();
	void Init();
	void recognizeWall(T_SensorData* sensorData);
	bool getCheckedFrontWall();
	bool getCheckedLeftWall();
	bool getCheckedRightWall();
	bool getCheckedBackWall();
	bool getBlockedFrontWall();
	bool getBlockedLeftWall();
	bool getBlockedRightWall();
	bool getBlockedBackWall();

private:
	void printDebug();
};


#endif /* WallFinderH */
