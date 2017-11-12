//------------------------------------------------------------------------------------------------
// File: Automode.cpp
// Project: LG Exec Ed Program
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "Manualmode.h"
#include "robot_operation.h"
#include "WallFinder.h"
#include "Direction.h"

using namespace std;


static T_manualmode_command Command;
static RobotPosition *Position = NULL;
static AlgorithmController *AlgorithmCtrl = NULL;
//static T_robot_moving_direction MovingDirection;
static WallFinder wallData;

//static std::thread	  *TestingThread=NULL;		// For testing

//static void CallBabckToGetEWSNDirectionManualmode(int ewsnDirection, T_algorithm_result result);


Manualmode::Manualmode(RobotPosition *position, AlgorithmController *algCtrl) {
	Position = position;
	init();
	AlgorithmCtrl = algCtrl;

}

void Manualmode::init() {
	Command = MANUALMODE_CMD_READY;
	robot_operation_manual(ROBOT_OPERATION_DIRECTION_STOP);
}

void Manualmode::doOperation() {
	switch (Command) {

	case MANUALMODE_CMD_READY:
		// Do nothing..
		break;

	case MANUALMODE_CMD_FORWARD:
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_FORWARD);
		break;

	case MANUALMODE_CMD_BACK:
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_BACKWARD);
		break;

	case MANUALMODE_CMD_TURN_LEFT:
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_LEFT);
		break;

	case MANUALMODE_CMD_TURN_RIGHT:
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_RIGHT);
		break;

	case MANUALMODE_CMD_STOP:
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_STOP);
		break;

	case MANUALMODE_CMD_CELL_MOVED:
//		Position->SuccessToMove();
//		AlgorithmCtrl->SendRobotCell(Position, 0, 0, 0, &wallData, (fp_ewsn_direction_result)CallBabckToGetEWSNDirectionManualmode);
		break;

	case MANUALMODE_CMD_CAMERA_LEFT:
		robot_operation_cam_manual(ROBOT_CAM_DIRECTION_LEFT);
		break;

	case MANUALMODE_CMD_CAMERA_RIGHT:
		robot_operation_cam_manual(ROBOT_CAM_DIRECTION_RIGHT);
		break;

	case MANUALMODE_CMD_CAMERA_UPPER:
		robot_operation_cam_manual(ROBOT_CAM_DIRECTION_UP);
		break;

	case MANUALMODE_CMD_CAMERA_LOWER:
		robot_operation_cam_manual(ROBOT_CAM_DIRECTION_DOWN);
		break;

	case MANUALMODE_CMD_CAMERA_CENTER:
		robot_operation_cam_manual(ROBOT_CAM_DIRECTION_CENTER);
		break;

	case MANUALMODE_CMD_CAMERA_LEFT_WALL:
		robot_operation_cam_manual(ROBOT_CAM_DIRECTION_LEFT_SIGN);
		break;

	case MANUALMODE_CMD_CAMERA_RIGHT_WALL:
		robot_operation_cam_manual(ROBOT_CAM_DIRECTION_RIGHT_SIGN);
		break;

	default:
		printf("Unresolved status(%d).\n", Command);
		break;
	}

	// Every command should send robot_operation at once. Therefore Command should return to ready.
	Command = MANUALMODE_CMD_READY;
}

//void Manualmode::setAlgorithmCtrl(AlgorithmController *algCtrl) {
//	AlgorithmCtrl = algCtrl;
//}

void Manualmode::setCommand(T_manualmode_command command) {
	Command = command;
}



//-----------------------------------------------------------------
// END Automode
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
