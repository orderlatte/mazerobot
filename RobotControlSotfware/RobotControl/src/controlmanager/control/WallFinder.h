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
	bool ConfirmedFrontWall;
	bool ConfirmedLeftWall;
	bool ConfirmedRightWall;
	bool ConfirmedBackWall;

public:
	WallFinder();
	void recognizeWall(T_SensorData* sensorData);
	bool getCheckedFrontWall();
	bool getCheckedLeftWall();
	bool getCheckedRightWall();
	bool getCheckedBackWall();
	bool getConfirmedFrontWall();
	bool getConfirmedLeftWall();
	bool getConfirmedRightWall();
	bool getConfirmedBackWall();
};


#endif /* WallFinderH */
