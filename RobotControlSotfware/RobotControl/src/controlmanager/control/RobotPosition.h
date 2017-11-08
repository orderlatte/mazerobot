/*
 * RobotPosition.h
 *
 *  Created on: November 6, 2017
 *      Author: lg
 */

#ifndef RobotPositionH
#define RobotPositionH

#include "Direction.h"


class RobotPosition {

private:
	int CurrentX;
	int CurrentY;
	int CurrentDirection;
	int NextDirection;

public:
	RobotPosition();
	T_robot_moving_direction SetEWSNDirectionToMove(int nextDirection);
	void SetDirectionToMove(T_robot_moving_direction nextDirection);
	void SuccessToMove();
	int getX();
	int getY();
	int getCurrentEWSNDirection();

private:
	T_robot_moving_direction GetEastDirectionToMove(int direction);
	T_robot_moving_direction GetWestDirectionToMove(int direction);
	T_robot_moving_direction GetSouthDirectionToMove(int direction);
	T_robot_moving_direction GetNorthDirectionToMove(int direction);
	void SetEastDirectionToMove(T_robot_moving_direction nextDirection);
	void SetWestDirectionToMove(T_robot_moving_direction nextDirection);
	void SetSouthDirectionToMove(T_robot_moving_direction nextDirection);
	void SetNorthDirectionToMove(T_robot_moving_direction nextDirection);
	void PrintDebugLog();
};


#endif /* RobotPositionH */
