//------------------------------------------------------------------------------------------------
// File: RobotStatus.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Send and receives OpenCV Mat Images in a UDP message commpressed as Jpeg images 
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "RobotStatus.h"

using namespace std;


RobotStatus::RobotStatus() {
	CurrentX = 10;
	CurrentY = 10;
	CurrentDirection = NORTH;
	NextDirection = NORTH;
}

T_robot_moving_direction RobotStatus::SetDirectionToMove(int nextDirection) {
	printf("NextDirection is %d", nextDirection);

	NextDirection = nextDirection;

	switch (CurrentDirection) {
	case EAST:
		return GetEastDirectionToMove(nextDirection);
	case WEST:
		return GetWestDirectionToMove(nextDirection);
	case SOUTH:
		return GetSouthDirectionToMove(nextDirection);
	case NORTH:
		return GetNorthDirectionToMove(nextDirection);
	default:
		printf("Fault - Unknown next direction: %d\n", nextDirection);
		return ROBOT_MOVING_DIRECTION_ERROR;
	}
}

void RobotStatus::SuccessToMove() {
	switch(NextDirection) {
	case EAST:
		CurrentX += 1;
		break;
	case WEST:
		CurrentX -= 1;
		break;
	case SOUTH:
		CurrentY += 1;
		break;
	case NORTH:
		CurrentY -= 1;
		break;
	default:
		printf("Next direction is invalid! (%d)", NextDirection);
		break;
	}

	CurrentDirection = NextDirection;

	PrintDebugLog();
}

void RobotStatus::PrintDebugLog() {
	printf("CurrentX: %d\n", CurrentX);
	printf("CurrentY: %d\n", CurrentY);
	printf("CurrentDirection: %d\n", CurrentDirection);
	printf("NextDirection: %d\n", NextDirection);
}



T_robot_moving_direction RobotStatus::GetEastDirectionToMove(int direction) {
	switch (direction) {
	case EAST:
		return ROBOT_MOVING_DIRECTION_FORWARD;
	case WEST:
		return ROBOT_MOVING_DIRECTION_BACK;
	case SOUTH:
		return ROBOT_MOVING_DIRECTION_RIGHT;
	case NORTH:
		return ROBOT_MOVING_DIRECTION_LEFT;
	default:
		printf("Fault - Unknown direction: %d\n", direction);
		return ROBOT_MOVING_DIRECTION_ERROR;
	}
}

T_robot_moving_direction RobotStatus::GetWestDirectionToMove(int direction) {
	switch (direction) {
	case EAST:
		return ROBOT_MOVING_DIRECTION_BACK;
	case WEST:
		return ROBOT_MOVING_DIRECTION_FORWARD;
	case SOUTH:
		return ROBOT_MOVING_DIRECTION_LEFT;
	case NORTH:
		return ROBOT_MOVING_DIRECTION_RIGHT;
	default:
		printf("Fault - Unknown direction: %d\n", direction);
		return ROBOT_MOVING_DIRECTION_ERROR;
	}
}

T_robot_moving_direction RobotStatus::GetSouthDirectionToMove(int direction) {
	switch (direction) {
	case EAST:
		return ROBOT_MOVING_DIRECTION_LEFT;
	case WEST:
		return ROBOT_MOVING_DIRECTION_RIGHT;
	case SOUTH:
		return ROBOT_MOVING_DIRECTION_FORWARD;
	case NORTH:
		return ROBOT_MOVING_DIRECTION_BACK;
	default:
		printf("Fault - Unknown direction: %d\n", direction);
		return ROBOT_MOVING_DIRECTION_ERROR;
	}
}

T_robot_moving_direction RobotStatus::GetNorthDirectionToMove(int direction) {
	switch (direction) {
	case EAST:
		return ROBOT_MOVING_DIRECTION_RIGHT;
	case WEST:
		return ROBOT_MOVING_DIRECTION_LEFT;
	case SOUTH:
		return ROBOT_MOVING_DIRECTION_BACK;
	case NORTH:
		return ROBOT_MOVING_DIRECTION_FORWARD;
	default:
		printf("Fault - Unknown direction: %d\n", direction);
		return ROBOT_MOVING_DIRECTION_ERROR;
	}
}

//-----------------------------------------------------------------
// END RobotStatus
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
