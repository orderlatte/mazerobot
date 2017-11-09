//------------------------------------------------------------------------------------------------
// File: RobotPosition.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Send and receives OpenCV Mat Images in a UDP message commpressed as Jpeg images 
//------------------------------------------------------------------------------------------------
#include "RobotPosition.h"
#include "Direction.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;


RobotPosition::RobotPosition() {
	CurrentX = 5;
	CurrentY = 5;
	CurrentDirection = NORTH;
	NextDirection = NORTH;
}

T_robot_moving_direction RobotPosition::SetEWSNDirectionToMove(int nextDirection) {
	printf("SetEWSNDirectionToMove() - NextEWSNDirection is %d\n", nextDirection);

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
		printf("SetEWSNDirectionToMove() - Fault - Unknown next direction: %d\n", nextDirection);
		return ROBOT_MOVING_DIRECTION_ERROR;
	}
}

void RobotPosition::SuccessToMove() {
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
		printf("SuccessToMove() - Next direction is invalid! (%d)", NextDirection);
		break;
	}

	CurrentDirection = NextDirection;

	PrintDebugLog();
}

int RobotPosition::getCurrentEWSNDirection() {
	printf("getCurrentEWSNDirection() - CurrentDirection: %d\n", CurrentDirection);
	return CurrentDirection;
}

void RobotPosition::PrintDebugLog() {
	printf("CurrentX: %d\n", CurrentX);
	printf("CurrentY: %d\n", CurrentY);
	printf("CurrentDirection: %d\n", CurrentDirection);
	printf("NextDirection: %d\n", NextDirection);
}



T_robot_moving_direction RobotPosition::GetEastDirectionToMove(int direction) {
	switch (direction) {
	case EAST:
		return ROBOT_MOVING_DIRECTION_FORWARD;
	case WEST:
		return ROBOT_MOVING_DIRECTION_BACK;
	case SOUTH:
		return ROBOT_MOVING_DIRECTION_RIGHT_FORWARD;
	case NORTH:
		return ROBOT_MOVING_DIRECTION_LEFT_FORWARD;
	default:
		printf("GetEastDirectionToMove() - Fault - Unknown direction: %d\n", direction);
		return ROBOT_MOVING_DIRECTION_ERROR;
	}
}

T_robot_moving_direction RobotPosition::GetWestDirectionToMove(int direction) {
	switch (direction) {
	case EAST:
		return ROBOT_MOVING_DIRECTION_BACK;
	case WEST:
		return ROBOT_MOVING_DIRECTION_FORWARD;
	case SOUTH:
		return ROBOT_MOVING_DIRECTION_LEFT_FORWARD;
	case NORTH:
		return ROBOT_MOVING_DIRECTION_RIGHT_FORWARD;
	default:
		printf("GetWestDirectionToMove() - Fault - Unknown direction: %d\n", direction);
		return ROBOT_MOVING_DIRECTION_ERROR;
	}
}

T_robot_moving_direction RobotPosition::GetSouthDirectionToMove(int direction) {
	switch (direction) {
	case EAST:
		return ROBOT_MOVING_DIRECTION_LEFT_FORWARD;
	case WEST:
		return ROBOT_MOVING_DIRECTION_RIGHT_FORWARD;
	case SOUTH:
		return ROBOT_MOVING_DIRECTION_FORWARD;
	case NORTH:
		return ROBOT_MOVING_DIRECTION_BACK;
	default:
		printf("GetSouthDirectionToMove() - Fault - Unknown direction: %d\n", direction);
		return ROBOT_MOVING_DIRECTION_ERROR;
	}
}

T_robot_moving_direction RobotPosition::GetNorthDirectionToMove(int direction) {
	switch (direction) {
	case EAST:
		return ROBOT_MOVING_DIRECTION_RIGHT_FORWARD;
	case WEST:
		return ROBOT_MOVING_DIRECTION_LEFT_FORWARD;
	case SOUTH:
		return ROBOT_MOVING_DIRECTION_BACK;
	case NORTH:
		return ROBOT_MOVING_DIRECTION_FORWARD;
	default:
		printf("GetNorthDirectionToMove() - Fault - Unknown direction: %d\n", direction);
		return ROBOT_MOVING_DIRECTION_ERROR;
	}
}


void RobotPosition::SetDirectionToMove(T_robot_moving_direction nextDirection) {
	printf("SetDirectionToMove() - Next direction is %d\n", nextDirection);

	switch (CurrentDirection) {
	case EAST:
		SetEastDirectionToMove(nextDirection);
		break;
	case WEST:
		SetWestDirectionToMove(nextDirection);
		break;
	case SOUTH:
		SetSouthDirectionToMove(nextDirection);
		break;
	case NORTH:
		SetNorthDirectionToMove(nextDirection);
		break;
	default:
		printf("SetDirectionToMove() - It's not supported EWSN direction.(%d)\n", CurrentDirection);
		break;
	}
}

void RobotPosition::SetEastDirectionToMove(T_robot_moving_direction nextDirection) {
	switch(nextDirection) {
	case ROBOT_MOVING_DIRECTION_FORWARD:
		NextDirection = EAST;
		break;
	case ROBOT_MOVING_DIRECTION_BACK:
		NextDirection = WEST;
		break;
	case ROBOT_MOVING_DIRECTION_LEFT_FORWARD:
		NextDirection = NORTH;
		break;
	case ROBOT_MOVING_DIRECTION_RIGHT_FORWARD:
		NextDirection = SOUTH;
		break;
	default:
		printf("SetEastDirectionToMove() - It's not supported direction(%d)\n", nextDirection);
		break;
	}
}

void RobotPosition::SetWestDirectionToMove(T_robot_moving_direction nextDirection) {
	switch(nextDirection) {
	case ROBOT_MOVING_DIRECTION_FORWARD:
		NextDirection = WEST;
		break;
	case ROBOT_MOVING_DIRECTION_BACK:
		NextDirection = EAST;
		break;
	case ROBOT_MOVING_DIRECTION_LEFT_FORWARD:
		NextDirection = SOUTH;
		break;
	case ROBOT_MOVING_DIRECTION_RIGHT_FORWARD:
		NextDirection = NORTH;
		break;
	default:
		printf("SetWestDirectionToMove() - It's not supported direction(%d)\n", nextDirection);
		break;
	}
}

void RobotPosition::SetSouthDirectionToMove(T_robot_moving_direction nextDirection) {
	switch(nextDirection) {
	case ROBOT_MOVING_DIRECTION_FORWARD:
		NextDirection = SOUTH;
		break;
	case ROBOT_MOVING_DIRECTION_BACK:
		NextDirection = NORTH;
		break;
	case ROBOT_MOVING_DIRECTION_LEFT_FORWARD:
		NextDirection = EAST;
		break;
	case ROBOT_MOVING_DIRECTION_RIGHT_FORWARD:
		NextDirection = WEST;
		break;
	default:
		printf("SetSouthDirectionToMove() - It's not supported direction(%d)\n", nextDirection);
		break;
	}
}

void RobotPosition::SetNorthDirectionToMove(T_robot_moving_direction nextDirection) {
	switch(nextDirection) {
	case ROBOT_MOVING_DIRECTION_FORWARD:
		NextDirection = NORTH;
		break;
	case ROBOT_MOVING_DIRECTION_BACK:
		NextDirection = SOUTH;
		break;
	case ROBOT_MOVING_DIRECTION_LEFT_FORWARD:
		NextDirection = WEST;
		break;
	case ROBOT_MOVING_DIRECTION_RIGHT_FORWARD:
		NextDirection = EAST;
		break;
	default:
		printf("SetNorthDirectionToMove() - It's not supported direction(%d)\n", nextDirection);
		break;
	}
}

int RobotPosition::getX() {
	return CurrentX;
}

int RobotPosition::getY() {
	return CurrentY;
}

//-----------------------------------------------------------------
// END RobotPosition
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
