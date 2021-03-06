//------------------------------------------------------------------------------------------------
// File: AlgorithmController.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstring>
#include <mutex>
#include "AlgorithmController.h"
#include "Direction.h"

using namespace std;

static TTcpConnectedPort *TcpConnectedPort = NULL;
static unsigned char *mapBuff = NULL;
static int mapBuffSize;
static unsigned char *robotCellBuff = NULL;
static int robotCellSize;
static unsigned char *robotEWSNBuff = NULL;
static int robotEWSNSize;
static AlgorithmController *AlgCtrl = NULL;
static StartingPoint *StartingData = NULL;

static std::thread *TestingThread = NULL;		// For testing
static std::mutex algorithmmutex;


AlgorithmController::AlgorithmController(StartingPoint *starting) {

	mapBuffSize = sizeof(unsigned char) + MAP_SIZE;
	mapBuff = (unsigned char *)malloc(mapBuffSize);
	memset(mapBuff, 0x00, mapBuffSize);

	robotCellSize = sizeof(unsigned char) + sizeof(double) + sizeof(int);
	robotCellBuff = (unsigned char *) malloc(robotCellSize);
	memset(robotCellBuff, 0x00, robotCellSize);

	robotEWSNSize = sizeof(unsigned char) * 3;
	robotEWSNBuff = (unsigned char *) malloc(robotEWSNSize);
	memset(robotEWSNBuff, 0x00, robotEWSNSize);

	StartingData = starting;

	AlgCtrl = this;
}

bool AlgorithmController::Open() {
	if ((TcpConnectedPort=OpenTcpConnection((const char*)"127.0.0.1",(const char*)"31000")) == NULL) {
		printf("Open() - OpenTcpConnection is failed!\n");
		return false;
	}

	return true;
}

void AlgorithmController::SendRobotCell(RobotPosition *robotPosition, FloorFinder *floor, WallFinder *wall, fp_ewsn_direction_result fp) {
	TestingThread = new std::thread(&AlgorithmController::SendRobotCellThread, this, robotPosition, floor, wall, fp);
}

void AlgorithmController::SendRobotCellThread(RobotPosition *robotPosition, FloorFinder *floor, WallFinder *wall, fp_ewsn_direction_result fp) {
	int CurrentEWSNDirection = robotPosition->GetCurrentEWSNDirection();
	short *tmpPosition = 0x00;
	int NextEWSNDirection = NORTH;
	T_algorithm_result Result = ALGORITHM_RESULT_ERROR;
	int index = 0;
	int redDotIndex = 0;
	int sign_type = 0;
	int sign_wall_position = 0;
	int currentPositionX = 0;
	int currentPositionY = 0;
//	T_SensorData sensorData = {0, };

	// To prevent race condition
	algorithmmutex.lock();

	wall->Init();
	wall->recognizeWall();

	// Assign call back function
	fpEWSNDirectionResult = fp;

	// Command
	robotCellBuff[0] = 0x4;

	// Robot Cell
	robotCellBuff[1] = (unsigned char) CurrentEWSNDirection;	// robot EWSN direction


	currentPositionX = robotPosition->GetX();
	currentPositionY = robotPosition->GetY();
	
	robotCellBuff[3] = 0x00;
	
	// Goal position
	if (floor->isAlreadyFoundedGoal(currentPositionX, currentPositionY) == true) {
		robotCellBuff[3] = 0x02;
	}

	// Starting position
	if (StartingData->isStartingPoint(currentPositionX, currentPositionY) == true) {
		robotCellBuff[3] = 0x01;
	} else {
		;//robotCellBuff[3] = 0x00;
	}

	redDotIndex = floor->getRedDotSign(currentPositionX, currentPositionY, &sign_type, &sign_wall_position);
	if (redDotIndex < 0) {
		robotCellBuff[2] = (unsigned char) 0x0;
		robotCellBuff[4] = (unsigned char) 0x0;
		robotCellBuff[5] = (unsigned char) 0x0;
	} else {
		robotCellBuff[2] = (unsigned char) sign_wall_position;
		robotCellBuff[4] = (unsigned char) sign_type;
		robotCellBuff[5] = (unsigned char) 0x1;
	}

	floor->RedDot = false;

	robotCellBuff[6] = wall->getCheckedWalls(CurrentEWSNDirection);
	robotCellBuff[7] = wall->getBlockedWalls(CurrentEWSNDirection);
	robotCellBuff[8] = 0x1;

	tmpPosition = (short *)&robotCellBuff[10];
	*tmpPosition = (short)currentPositionX;

	tmpPosition = (short *)&robotCellBuff[12];
	*tmpPosition = (short)currentPositionY;

	// For debugging
	printf("SendRobotCell() - robotCellBuff: 0x");
	for (index = 0; index < robotCellSize; index++) {
		printf("%1x", robotCellBuff[index]);
	}
	printf("\n");

	if (TcpConnectedPort == NULL) {
		printf("SendRobotCellThread() - TcpConnectedPort is NULL! Try to connect again.\n");
		if (Open() == false) {
//			printf("SendRobotCellThread() - Open() is failed!\n");
			goto cleanup;
		}
	}

	if (WriteDataTcp(TcpConnectedPort, robotCellBuff, robotCellSize) == -1) {
		printf("SendRobotCell() - WriteDataTcp() is failed!\n");
		goto cleanup;
	}

	if (ReadDataTcp(TcpConnectedPort, robotEWSNBuff, robotEWSNSize) != robotEWSNSize) {
		printf("SendRobotCell() - ReadDataTcp() is failed!\n");
		goto cleanup;
	}

	// For debugging
	printf("SendRobotCell() - robotEWSNBuff: 0x");
	for (index = 0; index < robotEWSNSize; index++) {
		printf("%1x", robotEWSNBuff[index]);
	}
	printf("\n");

	if (robotEWSNBuff[0] != 0x8) {
		printf("SendRobotCell() - robotEWSNBuff[0] is not 0x08. (%d)\n", mapBuff[0]);
		goto cleanup;
	}

	NextEWSNDirection = GetNextDirection(robotEWSNBuff[1]);
	Result = GetResultFromAlgorithm(robotEWSNBuff[2]);

cleanup:

	fpEWSNDirectionResult(NextEWSNDirection, Result);

	// Unlock mutex due to network connection, fpEWSNDirectionResult and other things are static variables which are shared automode and manual mode
	algorithmmutex.unlock();
}

//T_SensorData AlgorithmController::GetSensorData() {
//
//}

int AlgorithmController::GetNextDirection(unsigned char direction) {
	switch (direction) {

	case (unsigned char)0x1:
		return EAST;

	case (unsigned char)0x2:
		return WEST;

	case (unsigned char)0x4:
		return SOUTH;

	case (unsigned char)0x8:
		return NORTH;

	default:
		printf("GetNextDirection() - It's not supported direction! (%x)\n", direction);
		return NORTH;
	}
}

T_algorithm_result AlgorithmController::GetResultFromAlgorithm(unsigned char result) {
	switch (result) {

	case (unsigned char)0x0:
		return ALGORITHM_RESULT_OK;		// Success

	case (unsigned char)0x1:
		return ALGORITHM_RESULT_FULLY_MAPPED;		// Fully mapping

	case (unsigned char)0x2:
		return ALGORITHM_RESULT_ERROR;		// Error

	default:
		printf("GetResultFromAlgorithm() - It's not supported direction! (%x)\n", result);
		return ALGORITHM_RESULT_ERROR;
	}
}

static void RequestMap(unsigned char* map) {
	unsigned char request = 0x1;
	int index = 0;
	int printSize = 0;

	// To prevent race condition
	algorithmmutex.lock();

	if (TcpConnectedPort == NULL) {
		printf("SendRobotCellThread() - TcpConnectedPort is NULL!\n");
//		if (AlgCtrl->Open() == false) {
//			printf("SendRobotCellThread() - Open() is failed!\n");
//			goto cleanup;
//		}
		goto cleanup;
	}

	if (WriteDataTcp(TcpConnectedPort, &request, sizeof(unsigned char)) == -1) {
		printf("RequestMap() - WriteDataTcp() is failed!\n");
		goto cleanup;
	}

	if (ReadDataTcp(TcpConnectedPort, mapBuff, mapBuffSize) != mapBuffSize) {
		printf("RequestMap() - ReadDataTcp() is failed!\n");
		goto cleanup;
	}

	// For debugging
//	printf("RequestMap() - mapBuff: 0x");
//	printSize = sizeof(unsigned char) + (sizeof(double) * 5);
//	for (index = 0; index < printSize ; index++) {
//		printf("%1x", mapBuff[index]);
//	}
//	printf("\n");

	if (mapBuff[0] != 0x2) {
		printf("RequestMap() - buff[0] is not 0x02. (%d)\n", mapBuff[0]);
		goto cleanup;
	}

	std::memcpy(map, mapBuff+sizeof(unsigned char), MAP_SIZE);


cleanup:
	// To prevent race condition
	algorithmmutex.unlock();
}

fp_getMap AlgorithmController::GetMapFP() {
	return RequestMap;
}


void AlgorithmController::Close() {


	if (TcpConnectedPort == NULL) {
		return;
	}

	if (TcpConnectedPort->ConnectedFd != BAD_SOCKET_FD) {
		CLOSE_SOCKET(TcpConnectedPort->ConnectedFd);
		TcpConnectedPort->ConnectedFd=BAD_SOCKET_FD;
	}

	delete TcpConnectedPort;
	TcpConnectedPort = NULL;

	if (mapBuff != NULL) {
		free(mapBuff);
		mapBuff = NULL;
	}

	if (robotCellBuff != NULL) {
		free(robotCellBuff);
		robotCellBuff = NULL;
	}
}

void AlgorithmController::Reset(unsigned char algorithm) {
	int resetSize = sizeof(unsigned char) * 2;
	unsigned char reset[resetSize] = {0, };
	unsigned char response = 0x0;

	reset[0] = 0x9;
	reset[1] = algorithm;

	// To prevent race condition
	algorithmmutex.lock();

	if (TcpConnectedPort == NULL) {
		printf("Reset() - TcpConnectedPort is NULL!\n");
//		if (AlgCtrl->Open() == false) {
//			printf("SendRobotCellThread() - Open() is failed!\n");
//			goto cleanup;
//		}
		goto cleanup;
	}

	if (WriteDataTcp(TcpConnectedPort, reset, resetSize) == -1) {
		printf("Reset() - WriteDataTcp() is failed!\n");
		goto cleanup;
	}

	if (ReadDataTcp(TcpConnectedPort, &response, sizeof(unsigned char)) != sizeof(unsigned char)) {
		printf("Reset() - ReadDataTcp() is failed!\n");
		goto cleanup;
	}

	// For debugging
//	printf("RequestMap() - mapBuff: 0x");
//	printSize = sizeof(unsigned char) + (sizeof(double) * 5);
//	for (index = 0; index < printSize ; index++) {
//		printf("%1x", mapBuff[index]);
//	}
//	printf("\n");

	if (response != 0x0) {
		printf("Reset() - response is not 0x0. (%d)\n", mapBuff[0]);
		goto cleanup;
	}

cleanup:
	// To prevent race condition
	algorithmmutex.unlock();
}



//-----------------------------------------------------------------
// END AlgorithmController
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
