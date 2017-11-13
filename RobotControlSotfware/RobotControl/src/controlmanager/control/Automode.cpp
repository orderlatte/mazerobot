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
#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.
#include <wiringPi.h>
#endif //UBUNTU


using namespace std;


static T_automode_status Status;
static RobotPosition *Position = NULL;
static T_robot_moving_direction MovingDirection;
static WallFinder wallData;
static AlgorithmController *AlgorithmCtrl = NULL;
static bool FullyMappingCompleted;
static fp_automode_fail fpAutomodeFail;
static FloorFinder *FloorData;
static NextPositionSender *NextPosition;

//static std::thread	  *TestingThread=NULL;		// For testing

static void CallBabckToGetEWSNDirectionAutomode(int ewsnDirection, T_algorithm_result result);
static void CallBackRobotTurned();
static void CallBackRobotMoved();


Automode::Automode(RobotPosition *position, fp_automode_fail fp, AlgorithmController *algCtrl, FloorFinder *floor, NextPositionSender *positionSender) {
	Position = position;
	fpAutomodeFail = fp;
	init();
	AlgorithmCtrl = algCtrl;
	FloorData = floor;
	NextPosition = positionSender;
}

void Automode::init() {
	Status = AUTOMODE_STATUS_READY;
	MovingDirection = ROBOT_MOVING_DIRECTION_STOP;
	FullyMappingCompleted = false;
	robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
	robot_operation_cam_manual(ROBOT_CAM_DIRECTION_LINE);
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
	printf("doReady() is called!\n");

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

	case ROBOT_MOVING_DIRECTION_BACK_FORWARD:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_BACKWARD);
		printf("doReceivedMovingDirection() - Robot is moving backward.\n");
		Status = AUTOMODE_STATUS_TURNING;
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

	case ROBOT_MOVING_DIRECTION_BACK_FORWARD:
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_FORWARD);	// Backward
		printf("doTurned() - Robot is turned back and is moving forward.\n");
		Status = AUTOMODE_STATUS_MOVING;
		break;

	case ROBOT_MOVING_DIRECTION_FORWARD:
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

	case ROBOT_MOVING_DIRECTION_BACK_FORWARD:
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

	printf("position = %d, type = %d\n", FloorData->Sign_position, FloorData->Sign_type);
	AlgorithmCtrl->SendRobotCell(Position, FloorData->Sign_position, FloorData->Sign_type, FloorData, &wallData, (fp_ewsn_direction_result)CallBabckToGetEWSNDirectionAutomode);

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
	int nextX = 0;
	int nextY = 0;

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
		return;
	}

	MovingDirection = Position->SetEWSNDirectionToMove(ewsnDirection);
	Status = AUTOMODE_STATUS_RECEIVED_MOVING_DIRECTION;

	// Calculate Next Position
	Position->GetNextPosition(&nextX, &nextY);
	NextPosition->SendPosition(nextX, nextY);
}

static void CallBackRobotTurned() {
//	robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);

	if (Status != AUTOMODE_STATUS_TURNING) {
		printf("CallBackRobotTurned() - Current status is not turning.\n");
		return;
	}

	Status = AUTOMODE_STATUS_TURNED;

	printf("CallBackRobotTurned() is called!\n");
}

static void CallBackRobotMoved() {
	if (Status != AUTOMODE_STATUS_MOVING) {
		printf("CallBackRobotMoved() - Current status is not moving.\n");
		return;
	}

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
		printf("doMoving() - RedDot is found.\n");

		Status = AUTOMODE_STATUS_RECOGNIZING_SIGN;
	}
}

void Automode::doMoved() {
	printf("doMoved() is called.\n");

//	sleep(2);		// For testing..
	Status = AUTOMODE_STATUS_READY;
}

void Automode::doRecognizingSign() {

//	printf("doRecognizingSign() is called.\n");
	
	#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.
	static unsigned long recognize_start_time;
	static unsigned char recognize_state;
	static unsigned char recognize_wall_cnt;
	
	int positon;
	

	if(recognize_state == 0)
	{
		recognize_start_time = micros();
		recognize_state = 1;
	}
	else if(recognize_state == 1)
	{
		robot_operation_manual(ROBOT_OPERATION_DIRECTION_BACKWARD);
		recognize_state = 2;
	}
	else if(recognize_state == 2)
	{
		if(micros() - recognize_start_time > (200*1000))
		{
			robot_operation_manual(ROBOT_OPERATION_DIRECTION_STOP);
			recognize_start_time = micros();
			recognize_state = 3;
		}
	}
	else if(recognize_state == 3)
	{
		if(recognize_wall_cnt == 0)
		{
			robot_operation_cam_manual(ROBOT_CAM_DIRECTION_CENTER);
		}
		else if(recognize_wall_cnt == 1)
		{
			robot_operation_cam_manual(ROBOT_CAM_DIRECTION_LEFT_SIGN);
		}
		else if(recognize_wall_cnt == 2)
		{
			robot_operation_cam_manual(ROBOT_CAM_DIRECTION_RIGHT_SIGN);
		}
		recognize_state = 4;
	}
	else if(recognize_state == 4)
	{
		if(micros() - recognize_start_time > (2000*1000))
		{
			recognize_start_time = micros();
			recognize_wall_cnt++;
			recognize_state = 3;
		}
	}

	if((FloorData->Sign_type != 0) && (recognize_state == 4) && (micros() - recognize_start_time > (500*1000)))
	{
		positon = Position->GetNextEWSNDirection();
		if(recognize_wall_cnt == 0)
		{
			FloorData->Sign_position = positon;
		}
		else if(recognize_wall_cnt == 1)
		{
			switch(positon)
			{
				case EAST:
					FloorData->Sign_position = NORTH;
					break;
				case WEST:
					FloorData->Sign_position = SOUTH;
					break;
				case SOUTH:
					FloorData->Sign_position = EAST;
					break;
				case NORTH:
					FloorData->Sign_position = WEST;
					break;
			}
		}
		else if(recognize_wall_cnt == 2)
		{
			switch(positon)
			{
				case EAST:
					FloorData->Sign_position = SOUTH;
					break;
				case WEST:
					FloorData->Sign_position = NORTH;
					break;
				case SOUTH:
					FloorData->Sign_position = WEST;
					break;
				case NORTH:
					FloorData->Sign_position = EAST;
					break;
			}
		}

		robot_operation_cam_manual(ROBOT_CAM_DIRECTION_LINE);
		Status = AUTOMODE_STATUS_RESUME_TRAVLE;
		recognize_state = 0;
		recognize_wall_cnt = 0;
	}

	if(recognize_wall_cnt > 3)
	{
		fpAutomodeFail();
		Status = AUTOMODE_STATUS_READY;
	}
	

	#endif //UBUNTU

}

void Automode::doWaitingForSignResult() {
	// Do nothing...

	printf("doWaitingForSignResult() is called.\n");

	// TODO: Below code should be move to thread callback
	Status = AUTOMODE_STATUS_RESUME_TRAVLE;
}

void Automode::doResumeTravel() {
	printf("doResumeTravel() is called.\n");

	FloorData->RedDotRecognize = false;

	robot_operation_manual(ROBOT_OPERATION_DIRECTION_FORWARD);
	usleep(900000);	// For testing...
	robot_operation_manual(ROBOT_OPERATION_DIRECTION_STOP);

	Position->SuccessToMove();

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

	if (Status == AUTOMODE_STATUS_MOVING) {
		robot_operation_auto(ROBOT_OPERATION_DIRECTION_STOP);
		Status = AUTOMODE_STATUS_READY;
		fpAutomodeFail();
	}
}

void Automode::avoidLeftWall() {
	printf("avoidLeftWall() is called!\n");

	switch (MovingDirection) {
	case ROBOT_MOVING_DIRECTION_FORWARD:
	case ROBOT_MOVING_DIRECTION_LEFT_FORWARD:
		robot_operation_meet_wall(ROBOT_OPERATION_DIRECTION_RIGHT);	// Right
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
	case ROBOT_MOVING_DIRECTION_RIGHT_FORWARD:
		robot_operation_meet_wall(ROBOT_OPERATION_DIRECTION_LEFT);	// Left
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
