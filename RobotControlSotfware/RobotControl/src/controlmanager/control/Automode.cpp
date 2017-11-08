//------------------------------------------------------------------------------------------------
// File: Automode.cpp
// Project: LG Exec Ed Program
//------------------------------------------------------------------------------------------------
#include "Automode.h"
#include "sensor_manager.h"
#include "WallFinder.h"
#include "robot_operation.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

using namespace std;


static T_automode_status Status;
static RobotPosition *Position;
static T_robot_moving_direction MovingDirection;
static WallFinder wallData;

static std::thread	  *TestingThread=NULL;		// For testing
static fp_robot_ewsn_direction fpEWSNDirectionCallBack;

static void CallBabckToGetEWSNDirection(int ewsnDirection);
static void CallBackRobotTurned();
static void CallBackRobotMoved();


Automode::Automode(RobotPosition *position) {
	Status = AUTOMODE_STATUS_READY;
	Position = position;
	MovingDirection = ROBOT_MOVING_DIRECTION_STOP;
	fpEWSNDirectionCallBack = &CallBabckToGetEWSNDirection;
}

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
	default:
		printf("Unresolved status(%d).\n", Status);
		break;
	}
}

void Automode::doReady() {
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
		Status = AUTOMODE_STATUS_MOVING;
		break;

	case ROBOT_MOVING_DIRECTION_LEFT_FORWARD:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_LEFT);	// Left
		Status = AUTOMODE_STATUS_TURNING;
		break;

	case ROBOT_MOVING_DIRECTION_RIGHT_FORWARD:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_RIGHT);	// Right
		Status = AUTOMODE_STATUS_TURNING;
		break;

	case ROBOT_MOVING_DIRECTION_BACK:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_BACKWARD);
		Status = AUTOMODE_STATUS_MOVING;
		break;

	case ROBOT_MOVING_DIRECTION_STOP:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
		Status = AUTOMODE_STATUS_READY;
		break;

	default:
		printf("Error! This is not supported direction(%d).\n", MovingDirection);
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
		Status = AUTOMODE_STATUS_READY;
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
		Status = AUTOMODE_STATUS_MOVING;
		break;

	case ROBOT_MOVING_DIRECTION_RIGHT_FORWARD:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);	// Right
		Status = AUTOMODE_STATUS_MOVING;
		break;

	case ROBOT_MOVING_DIRECTION_FORWARD:
	case ROBOT_MOVING_DIRECTION_BACK:
	case ROBOT_MOVING_DIRECTION_STOP:
	default:
		printf("doTurned() Error! This is not supported direction(%d).\n", MovingDirection);
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
		Status = AUTOMODE_STATUS_READY;
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
		printf("Robot could not handle in this case!\n");
		break;
	}
}

void Automode::sendRobotStatusToAlgorithm() {
	T_SensorData sensorData = get_sensor_data();

	wallData.Init();
	wallData.recognizeWall(&sensorData);

    // TODO: Send algorithm module
//	sendCurrentRobotStatus(&sensorData, &wallData, initialStarted);

	// TODO: Below code will be replaced with Maze algorithm
	TestingThread = new std::thread(&Automode::getTestNextDirectionForTesting, this, &wallData, fpEWSNDirectionCallBack);
}


// It's for testing..
void Automode::getTestNextDirectionForTesting(WallFinder *wallData, fp_robot_ewsn_direction callback) {
	int EWSN = Position->getCurrentEWSNDirection();

	printf("getTestNextDirectionForTesting() is called.\n");
	printf("getTestNextDirectionForTesting() - EWSN: %d\n", EWSN);

	usleep(20000);	// 20 milisecond

	switch (EWSN) {
	case EAST:
		if (wallData->getBlockedFrontWall() == false) {
			callback(EAST);
			return;
		}

		if (wallData->getBlockedLeftWall() == false) {
			callback(NORTH);
			return;
		}

		if (wallData->getBlockedRightWall() == false) {
			callback(SOUTH);
			return;
		}

		callback(WEST);
		return;

	case WEST:
		if (wallData->getBlockedFrontWall() == false) {
			callback(WEST);
			return;
		}

		if (wallData->getBlockedLeftWall() == false) {
			callback(SOUTH);
			return;
		}

		if (wallData->getBlockedRightWall() == false) {
			callback(NORTH);
			return;
		}

		callback(EAST);
		return;

	case SOUTH:
		if (wallData->getBlockedFrontWall() == false) {
			callback(SOUTH);
			return;
		}

		if (wallData->getBlockedLeftWall() == false) {
			callback(EAST);
			return;
		}

		if (wallData->getBlockedRightWall() == false) {
			callback(WEST);
			return;
		}

		callback(NORTH);
		return;

	case NORTH:
		if (wallData->getBlockedFrontWall() == false) {
			printf("getTestNextDirectionForTesting() - call north\n");
			callback(NORTH);
			return;
		}

		if (wallData->getBlockedLeftWall() == false) {
			printf("getTestNextDirectionForTesting() - call west\n");
			callback(WEST);
			return;
		}

		if (wallData->getBlockedRightWall() == false) {
			printf("getTestNextDirectionForTesting() - call east\n");
			callback(EAST);
			return;
		}

		printf("getTestNextDirectionForTesting() - call south\n");
		callback(SOUTH);
		return;

	default:
		printf("It is not supported EWSN direction(%d)!", EWSN);
		callback(NORTH);
		return;
	}
}

static void CallBabckToGetEWSNDirection(int ewsnDirection) {
	printf("CallBabckToGetEWSNDirection(%d) is called.\n", ewsnDirection);
	MovingDirection = Position->SetEWSNDirectionToMove(ewsnDirection);
	Status = AUTOMODE_STATUS_RECEIVED_MOVING_DIRECTION;
}

static void CallBackRobotTurned() {
//	robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
	Status = AUTOMODE_STATUS_TURNED;
}

static void CallBackRobotMoved() {
//	robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
	Status = AUTOMODE_STATUS_MOVED;
	Position->SuccessToMove();
}

void Automode::doMoving() {
	// Do nothing.. Keep current robot operation...
}

void Automode::doMoved() {
	// TODO: If there is red dot, move to recognize sign status.
	Status = AUTOMODE_STATUS_READY;
}

void Automode::doRecognizingSign() {
	// TODO: something..
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

	if (Status == AUTOMODE_STATUS_MOVING) {
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
		Status = AUTOMODE_STATUS_READY;
	}
}

void Automode::avoidLeftWall() {
	printf("avoidLeftWall() is called!\n");

	switch (MovingDirection) {
	case ROBOT_MOVING_DIRECTION_FORWARD:
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_RIGHT);	// Right
		usleep(100000);	// sleep 100 milliseconds
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);	// Left
		break;

//	case ROBOT_MOVING_DIRECTION_BACK:
//		robot_operation_manual(ROBOT_OPERATION_DIRECTION_LEFT);	// Left
//		usleep(500000);	// sleep 500 milliseconds
//		robot_operation_auto(ROBOT_MOVING_DIRECTION_BACK);	// Left
//		break;

	case ROBOT_MOVING_DIRECTION_LEFT_FORWARD:
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_RIGHT);	// Right
		usleep(100000);	// sleep 100 milliseconds
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);	// Left
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
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_LEFT);	// Left
		usleep(100000);	// sleep 100 milliseconds
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);	// Left
		break;

//	case ROBOT_MOVING_DIRECTION_BACK:
//		robot_operation_manual(ROBOT_OPERATION_DIRECTION_RIGHT);	// Right
//		usleep(100000);	// sleep 100 milliseconds
//		robot_operation_auto(ROBOT_MOVING_DIRECTION_BACK);	// Left
//		break;

	case ROBOT_MOVING_DIRECTION_RIGHT_FORWARD:
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_LEFT);	// Left
		usleep(100000);	// sleep 100 milliseconds
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);	// Left
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
