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
		// TODO: Call API of robot_operation
		break;

	case MANUALMODE_CMD_CAMERA_RIGHT:
		// TODO: Call API of robot_operation
		break;

	case MANUALMODE_CMD_CAMERA_UPPER:
		// TODO: Call API of robot_operation
		break;

	case MANUALMODE_CMD_CAMERA_LOWER:
		// TODO: Call API of robot_operation
		break;

	case MANUALMODE_CMD_CAMERA_CENTER:
		// TODO: Call API of robot_operation
		break;

	case MANUALMODE_CMD_CAMERA_LEFT_WALL:
		// TODO: Call API of robot_operation
		break;

	case MANUALMODE_CMD_CAMERA_RIGHT_WALL:
		// TODO: Call API of robot_operation
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

//static void CallBabckToGetEWSNDirectionManualmode(int ewsnDirection, T_algorithm_result result) {
//	printf("CallBabckToGetEWSNDirectionManualmode(%d) is called.\n", ewsnDirection);
//
//	if (result == ALGORITHM_RESULT_ERROR) {
//		printf("CallBabckToGetEWSNDirectionManualmode() - Error! Algorithm is not operated properly. Robot will be stopped.\n");
//		// There is no further action.. We could not do any more.
//		// TODO: Send UI that current status is error status.
//		Position->RecoverCoordinates();
//		return;
//	}
//
//	if (result == ALGORITHM_RESULT_FULLY_MAPPED) {
//		printf("CallBabckToGetEWSNDirectionManualmode() - Fully mapping is completed.\n");
//		// TODO: Send UI that map is fully mapped.
//		return;
//	}
//
//	MovingDirection = Position->SetEWSNDirectionToMove(ewsnDirection);
//	Command = MANUALMODE_CMD_READY;
//}


//-----------------------------------------------------------------
// END Automode
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
