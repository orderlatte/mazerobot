//------------------------------------------------------------------------------------------------
// File: Automode.cpp
// Project: LG Exec Ed Program
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "Automode.h"
#include "sensor_manager.h"
#include "robot_operation.h"
#include "WallFinder.h"
#include "Direction.h"

using namespace std;


static T_automode_status Status;
static RobotPosition *Position = NULL;
static T_robot_moving_direction MovingDirection;
static WallFinder wallData;
static AlgorithmController *AlgorithmCtrl = NULL;
static bool FullyMappingCompleted;
static fp_automode_fail fpAutomodeFail;
static FloorFinder *FloorData;

//static std::thread	  *TestingThread=NULL;		// For testing

static void CallBabckToGetEWSNDirectionAutomode(int ewsnDirection, T_algorithm_result result);
static void CallBackRobotTurned();
static void CallBackRobotMoved();


Automode::Automode(RobotPosition *position, fp_automode_fail fp, AlgorithmController *algCtrl, FloorFinder *floor) {
	Position = position;
	fpAutomodeFail = fp;
	init();
	AlgorithmCtrl = algCtrl;
	FloorData = floor;
}

void Automode::init() {
	Status = AUTOMODE_STATUS_READY;
	MovingDirection = ROBOT_MOVING_DIRECTION_STOP;
	FullyMappingCompleted = false;
	robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
}

void Automode::resume() {
	Position->SuccessToMove();
	init();
}

//void Automode::setAlgorithmCtrl(AlgorithmController *algCtrl) {
//	AlgorithmCtrl = algCtrl;
//}

void Automode::doOperation() {
	switch (Status) {
	case AUTOMODE_STATUS_READY:
		doReady();
		break;
	case AUTOMODE_STATUS_WAITING_FOR_DIRECTION:
		doWaitingForDirection();
		break;
	case AUTOMODE_STATUS_RECEIVED_MOVING_DIRECTION:
		doReceivedMovingDirection();
		break;
	case AUTOMODE_STATUS_TURNING:
		doTurning();
		break;
	case AUTOMODE_STATUS_TURNED:
		doTurned();
		break;
	case AUTOMODE_STATUS_MOVING:
		doMoving();
		break;
	case AUTOMODE_STATUS_MOVED:
		doMoved();
		break;
	case AUTOMODE_STATUS_RECOGNIZING_SIGN:
		doRecognizingSign();
		break;
	case AUTOMODE_STATUS_WATING_FOR_SIGN_RESULT:
		doWaitingForSignResult();
		break;
	case AUTOMODE_STATUS_RESUME_TRAVLE:
		doResumeTravel();
		break;
	default:
		printf("doOperation() - Unresolved status(%d).\n", Status);
		fpAutomodeFail();
		break;
	}
}

void Automode::doReady() {
	if (FullyMappingCompleted == true) {
		printf("doReady() - fully mapping is completed.\n");
		return;
	}

	sendRobotStatusToAlgorithm();
	Status = AUTOMODE_STATUS_WAITING_FOR_DIRECTION;
}

void Automode::doWaitingForDirection() {
	// Do nothing.. Keep current robot operation...
}

void Automode::doReceivedMovingDirection() {
	switch (MovingDirection) {

	case ROBOT_MOVING_DIRECTION_FORWARD:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);
		printf("doReceivedMovingDirection() - Robot is moving forward.\n");
		Status = AUTOMODE_STATUS_MOVING;
		break;

	case ROBOT_MOVING_DIRECTION_LEFT_FORWARD:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_LEFT);	// Left
		printf("doReceivedMovingDirection() - Robot is turning left.\n");
		Status = AUTOMODE_STATUS_TURNING;
		break;

	case ROBOT_MOVING_DIRECTION_RIGHT_FORWARD:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_RIGHT);	// Right
		printf("doReceivedMovingDirection() - Robot is turning right.\n");
		Status = AUTOMODE_STATUS_TURNING;
		break;

	case ROBOT_MOVING_DIRECTION_BACK:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_BACKWARD);
		printf("doReceivedMovingDirection() - Robot is moving backward.\n");
		Status = AUTOMODE_STATUS_MOVING;
		break;

	case ROBOT_MOVING_DIRECTION_STOP:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
		Status = AUTOMODE_STATUS_READY;
		break;

	default:
		printf("doReceivedMovingDirection() - Error! This is not supported direction(%d).\n", MovingDirection);
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
		Status = AUTOMODE_STATUS_READY;
		fpAutomodeFail();
		break;
	}
}

void Automode::doTurning() {
	// Do nothing.. Keep current robot operation...
}

void Automode::doTurned() {
	switch (MovingDirection) {

	case ROBOT_MOVING_DIRECTION_LEFT_FORWARD:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);	// Left
		printf("doTurned() - Robot is turned left and is moving forward.\n");
		Status = AUTOMODE_STATUS_MOVING;
		break;

	case ROBOT_MOVING_DIRECTION_RIGHT_FORWARD:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);	// Right
		printf("doTurned() - Robot is turned right and is moving forward.\n");
		Status = AUTOMODE_STATUS_MOVING;
		break;

	case ROBOT_MOVING_DIRECTION_FORWARD:
	case ROBOT_MOVING_DIRECTION_BACK:
	case ROBOT_MOVING_DIRECTION_STOP:
	default:
		printf("doTurned() - Error! This is not supported direction(%d).\n", MovingDirection);
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
		Status = AUTOMODE_STATUS_READY;
		fpAutomodeFail();
		break;
	}
}

void Automode::moveNextCell() {
	printf("moveNextCell() is called!\n");

	switch (MovingDirection) {
	case ROBOT_MOVING_DIRECTION_FORWARD:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);	// Forward
		break;

	case ROBOT_MOVING_DIRECTION_BACK:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_BACKWARD);	// Backward
		break;

	case ROBOT_MOVING_DIRECTION_LEFT_FORWARD:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_LEFT);	// Left
		break;

	case ROBOT_MOVING_DIRECTION_RIGHT_FORWARD:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_RIGHT);	// Right
		break;

	case ROBOT_MOVING_DIRECTION_STOP:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);	// Stop
		break;

	default:
		printf("moveNextCell() - Robot could not handle in this case!\n");
		fpAutomodeFail();
		break;
	}
}

void Automode::sendRobotStatusToAlgorithm() {
//	int point = 0x0; // Starting point, Goal

    // TODO: Ad sign information

	AlgorithmCtrl->SendRobotCell(Position, 0, 0, FloorData, &wallData, (fp_ewsn_direction_result)CallBabckToGetEWSNDirectionAutomode);

//	TestingThread = new std::thread(&Automode::getTestNextDirectionForTesting, this, &wallData, fpEWSNDirectionCallBack);
}


// It's for testing..
//void Automode::getTestNextDirectionForTesting(WallFinder *wallData, fp_ewsn_direction_result callback) {
//	int EWSN = Position->getCurrentEWSNDirection();
//
//	printf("getTestNextDirectionForTesting() is called.\n");
//	printf("getTestNextDirectionForTesting() - EWSN: %d\n", EWSN);
//
//	usleep(20000);	// 20 milisecond
//
//	switch (EWSN) {
//	case EAST:
//		if (wallData->getBlockedFrontWall() == false) {
//			callback(EAST);
//			return;
//		}
//
//		if (wallData->getBlockedLeftWall() == false) {
//			callback(NORTH);
//			return;
//		}
//
//		if (wallData->getBlockedRightWall() == false) {
//			callback(SOUTH);
//			return;
//		}
//
//		callback(WEST);
//		return;
//
//	case WEST:
//		if (wallData->getBlockedFrontWall() == false) {
//			callback(WEST);
//			return;
//		}
//
//		if (wallData->getBlockedLeftWall() == false) {
//			callback(SOUTH);
//			return;
//		}
//
//		if (wallData->getBlockedRightWall() == false) {
//			callback(NORTH);
//			return;
//		}
//
//		callback(EAST);
//		return;
//
//	case SOUTH:
//		if (wallData->getBlockedFrontWall() == false) {
//			callback(SOUTH);
//			return;
//		}
//
//		if (wallData->getBlockedLeftWall() == false) {
//			callback(EAST);
//			return;
//		}
//
//		if (wallData->getBlockedRightWall() == false) {
//			callback(WEST);
//			return;
//		}
//
//		callback(NORTH);
//		return;
//
//	case NORTH:
//		if (wallData->getBlockedFrontWall() == false) {
//			printf("getTestNextDirectionForTesting() - call north\n");
//			callback(NORTH);
//			return;
//		}
//
//		if (wallData->getBlockedLeftWall() == false) {
//			printf("getTestNextDirectionForTesting() - call west\n");
//			callback(WEST);
//			return;
//		}
//
//		if (wallData->getBlockedRightWall() == false) {
//			printf("getTestNextDirectionForTesting() - call east\n");
//			callback(EAST);
//			return;
//		}
//
//		printf("getTestNextDirectionForTesting() - call south\n");
//		callback(SOUTH);
//		return;
//
//	default:
//		printf("It is not supported EWSN direction(%d)!", EWSN);
//		callback(NORTH);
//		return;
//	}
//}

static void CallBabckToGetEWSNDirectionAutomode(int ewsnDirection, T_algorithm_result result) {
	printf("CallBabckToGetEWSNDirectionAutomode(%d) is called.\n", ewsnDirection);

	if (result == ALGORITHM_RESULT_ERROR) {
		printf("CallBabckToGetEWSNDirectionAutomode() - Error! Algorithm is not operated properly. Robot will be stopped.\n");
		fpAutomodeFail();
		Status = AUTOMODE_STATUS_READY;
		return;
	}

	if (result == ALGORITHM_RESULT_FULLY_MAPPED) {
		printf("Fully mapping is completed.\n");
		FullyMappingCompleted = true;
		Status = AUTOMODE_STATUS_READY;
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
		// TODO: Robot should transit to manual mode and the map is fully mapped.
		return;
	}

	MovingDirection = Position->SetEWSNDirectionToMove(ewsnDirection);
	Status = AUTOMODE_STATUS_RECEIVED_MOVING_DIRECTION;
}

static void CallBackRobotTurned() {
//	robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
	Status = AUTOMODE_STATUS_TURNED;

	printf("CallBackRobotTurned() is called!\n");
}

static void CallBackRobotMoved() {
	Status = AUTOMODE_STATUS_MOVED;
	if (FloorData->RedDot == true) {
		printf("CallBackRobotMoved() - Red dot is existed! Ignore callback robot moved.\n");
		return;
	}

	Position->SuccessToMove();

	printf("CallBackRobotMoved() is called!\n");
	// For testing...
//	robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
//	sleep(2);		// 2sec
}

void Automode::doMoving() {
	// Do nothing.. Keep current robot operation...
	if (FloorData->RedDot == true) {
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);	// Stop
		sleep(2);	// For testing...
		Status = AUTOMODE_STATUS_RECOGNIZING_SIGN;
	}
}

void Automode::doMoved() {
//	sleep(2);		// For testing..
	Status = AUTOMODE_STATUS_READY;
}

void Automode::doRecognizingSign() {
	// TODO: Thread start to recognize Sign.

	Status = AUTOMODE_STATUS_WATING_FOR_SIGN_RESULT;
}

void Automode::doWaitingForSignResult() {
	// Do nothing...

	// TODO: Below code should be move to thread callback
	Status = AUTOMODE_STATUS_RESUME_TRAVLE;
}

void Automode::doResumeTravel() {
	robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);
	usleep(500000);	// For testing...
	robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);

	Status = AUTOMODE_STATUS_READY;
}

fp_robot_turned Automode::getRobotTurnedFP() {
	return CallBackRobotTurned;
}

fp_robot_moved Automode::getRobotMovedFP() {
	return CallBackRobotMoved;
}

void Automode::setStatus(T_automode_status status) {
//	switch (status) {
//
//	case AUTOMODE_STATUS_READY:
//		Status = AUTOMODE_STATUS_READY;
//		break;
//
//	case AUTOMODE_STATUS_WAITING_FOR_DIRECTION:
//		if (Status == AUTOMODE_STATUS_READY) {
//			Status = AUTOMODE_STATUS_WAITING_FOR_DIRECTION;
//		} else {
//			printf("setStatus() - Previous status is not ready.\n");
//			Status = AUTOMODE_STATUS_READY;
//		}
//		break;
//
//	case AUTOMODE_STATUS_RECEIVED_MOVING_DIRECTION:
//		if (Status == AUTOMODE_STATUS_WAITING_FOR_DIRECTION) {
//			Status = AUTOMODE_STATUS_RECEIVED_MOVING_DIRECTION;
//		} else {
//			printf("setStatus() - Previous status is not waiting for moving direction.\n");
//			Status = AUTOMODE_STATUS_READY;
//		}
//		break;
//
//	case AUTOMODE_STATUS_TURNING:
//
//		break;
//	case AUTOMODE_STATUS_TURNED:
//		break;
//	case AUTOMODE_STATUS_MOVING:
//		break;
//	case AUTOMODE_STATUS_MOVED:
//		break;
//	case AUTOMODE_STATUS_RECOGNIZING_SIGN:
//		break;
//	default:
//		printf("setStatus() - Error! (%d)\n", status);
//		break;
//	}
}

void Automode::stopRobot() {
	printf("stopRobot() in Automode is called.(%d)\n", Status);

	// TODO: Stop and go to manual mode
//	if (Status == AUTOMODE_STATUS_MOVING) {
//		robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
//		Status = AUTOMODE_STATUS_READY;
//	}
}

void Automode::avoidLeftWall() {
	printf("avoidLeftWall() is called!\n");

	switch (MovingDirection) {
	case ROBOT_MOVING_DIRECTION_FORWARD:
		// TODO: replace new API of robot_operation
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_RIGHT);	// Right
//		usleep(100000);	// sleep 100 milliseconds
//		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);	// Left
		break;

//	case ROBOT_MOVING_DIRECTION_BACK:
//		robot_operation_manual(ROBOT_OPERATION_DIRECTION_LEFT);	// Left
//		usleep(500000);	// sleep 500 milliseconds
//		robot_operation_auto(ROBOT_MOVING_DIRECTION_BACK);	// Left
//		break;

	case ROBOT_MOVING_DIRECTION_LEFT_FORWARD:
		// TODO: replace new API of robot_operation
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_RIGHT);	// Right
//		usleep(100000);	// sleep 100 milliseconds
//		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);	// Left
		break;

	default:
		printf("Robot could not handle in this case!\n");
		break;
	}
}

void Automode::avoidRightWall() {
	printf("avoidRightWall() is called!\n");

	switch (MovingDirection) {
	case ROBOT_MOVING_DIRECTION_FORWARD:
		// TODO: replace new API of robot_operation
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_LEFT);	// Left
//		usleep(100000);	// sleep 100 milliseconds
//		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);	// Left
		break;

//	case ROBOT_MOVING_DIRECTION_BACK:
//		robot_operation_manual(ROBOT_OPERATION_DIRECTION_RIGHT);	// Right
//		usleep(100000);	// sleep 100 milliseconds
//		robot_operation_auto(ROBOT_MOVING_DIRECTION_BACK);	// Left
//		break;

	case ROBOT_MOVING_DIRECTION_RIGHT_FORWARD:
		// TODO: replace new API of robot_operation
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_LEFT);	// Left
//		usleep(100000);	// sleep 100 milliseconds
//		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);	// Left
		break;

	default:
		printf("Robot could not handle in this case!\n");
		break;
	}
}


//-----------------------------------------------------------------
// END Automode
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
