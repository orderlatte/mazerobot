/*
 * RobotStatus.h
 *
 *  Created on: November 6, 2017
 *      Author: lg
 */

#ifndef RobotStatusH
#define RobotStatusH

#include "Direction.h"

class RobotStatus {

private:
	int CurrentX;
	int CurrentY;
	int CurrentDirection;
	int NextDirection;

public:
	RobotStatus();
	T_robot_moving_direction SetDirectionToMove(int nextDirection);
	void SuccessToMove();

private:
	T_robot_moving_direction GetEastDirectionToMove(int direction);
	T_robot_moving_direction GetWestDirectionToMove(int direction);
	T_robot_moving_direction GetSouthDirectionToMove(int direction);
	T_robot_moving_direction GetNorthDirectionToMove(int direction);
	void PrintDebugLog();
};


#endif /* RobotStatusH */
