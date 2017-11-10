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
static unsigned char *mapBuff;
static int mapBuffSize;
static unsigned char *robotCellBuff;
static int robotCellSize;
static unsigned char *robotEWSNBuff;
static int robotEWSNSize;

static std::thread *TestingThread = NULL;		// For testing
static std::mutex algorithmmutex;


AlgorithmController::AlgorithmController(fp_ewsn_direction_result fp) {
	fpEWSNDirectionResult = fp;
//	SendMap = sendMap;

	mapBuffSize = sizeof(unsigned char) + MAP_SIZE;
	mapBuff = (unsigned char *)malloc(mapBuffSize);
	memset(mapBuff, 0x00, mapBuffSize);

	robotCellSize = sizeof(unsigned char) + sizeof(double) + sizeof(int);
	robotCellBuff = (unsigned char *) malloc(robotCellSize);
	memset(robotCellBuff, 0x00, robotCellSize);

	robotEWSNSize = sizeof(unsigned char) * 3;
	robotEWSNBuff = (unsigned char *) malloc(robotEWSNSize);
	memset(robotEWSNBuff, 0x00, robotEWSNSize);
}

void AlgorithmController::Open() {
	if ((TcpConnectedPort=OpenTcpConnection((const char*)"127.0.0.1",(const char*)"31000")) == NULL) {
		printf("Open() - OpenTcpConnection is failed!\n");
		return;
	}
}

void AlgorithmController::SendRobotCell(RobotPosition *robotPosition, int signPosition, int signType, int redDot, WallFinder *wall) {
	TestingThread = new std::thread(&AlgorithmController::SendRobotCellThread, this, robotPosition, signPosition, signType, redDot, wall);
}

void AlgorithmController::SendRobotCellThread(RobotPosition *robotPosition, int signPosition, int signType, int redDot, WallFinder *wall) {
	int CurrentEWSNDirection = robotPosition->getCurrentEWSNDirection();
	short *tmpPosition = 0x00;
	int NextEWSNDirection = 0;
	int Result = 0;
	int index = 0;

	// Command
	robotCellBuff[0] = 0x4;

	// Robot Cell
	robotCellBuff[1] = (unsigned char) CurrentEWSNDirection;	// robot EWSN direction
	robotCellBuff[2] = signPosition; // TODO: sign position
	robotCellBuff[3] = 0;
	robotCellBuff[4] = signType; // TODO: sign type
	robotCellBuff[5] = redDot;

	robotCellBuff[6] = wall->getCheckedWalls(CurrentEWSNDirection);
	robotCellBuff[7] = wall->getBlockedWalls(CurrentEWSNDirection);
	robotCellBuff[8] = 0x1;

	tmpPosition = (short *)&robotCellBuff[10];
	*tmpPosition = (short)robotPosition->getX();

	tmpPosition = (short *)&robotCellBuff[12];
	*tmpPosition = (short)robotPosition->getY();

	// For debugging
	printf("SendRobotCell() - robotCellBuff: 0x");
	for (index = 0; index < robotCellSize; index++) {
		printf("%1x", robotCellBuff[index]);
	}
	printf("\n");

	if (TcpConnectedPort == NULL) {
		printf("SendRobotCellThread() - TcpConnectedPort is NULL!\n");
		// TODO: Go to manual mode
		return;
	}

	// To prevent race condition
	algorithmmutex.lock();

	if (WriteDataTcp(TcpConnectedPort, robotCellBuff, robotCellSize) == -1) {
		printf("SendRobotCell() - WriteDataTcp() is failed!\n");
		return;
	}

	if (ReadDataTcp(TcpConnectedPort, robotEWSNBuff, robotEWSNSize) != robotEWSNSize) {
		printf("SendRobotCell() - ReadDataTcp() is failed!\n");
		return;
	}

	algorithmmutex.unlock();

	// For debugging
	printf("SendRobotCell() - robotEWSNBuff: 0x");
	for (index = 0; index < robotEWSNSize; index++) {
		printf("%1x", robotEWSNBuff[index]);
	}
	printf("\n");

	if (robotEWSNBuff[0] != 0x8) {
		printf("SendRobotCell() - robotEWSNBuff[0] is not 0x08. (%d)\n", mapBuff[0]);
		return;
	}

	NextEWSNDirection = GetNextDirection(robotEWSNBuff[1]);
	Result = GetResultFromAlgorithm(robotEWSNBuff[2]);

	fpEWSNDirectionResult(NextEWSNDirection, Result);
}

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

int AlgorithmController::GetResultFromAlgorithm(unsigned char result) {
	switch (result) {

	case (unsigned char)0x0:
		return 0;		// Success

	case (unsigned char)0x1:
		return 1;		// Fully mapping

	case (unsigned char)0x2:
		return 2;		// Error

	default:
		printf("GetResultFromAlgorithm() - It's not supported direction! (%x)\n", result);
		return 2;
	}
}

static void RequestMap(unsigned char* map) {
	unsigned char request = 0x1;
	int index = 0;
	int printSize = 0;

	if (TcpConnectedPort == NULL) {
		printf("SendRobotCellThread() - TcpConnectedPort is NULL!\n");
		// TODO: Go to manual mode
		return;
	}

	// To prevent race condition
	algorithmmutex.lock();

	if (WriteDataTcp(TcpConnectedPort, &request, sizeof(unsigned char)) == -1) {
		printf("RequestMap() - WriteDataTcp() is failed!\n");
		return;
	}

	if (ReadDataTcp(TcpConnectedPort, mapBuff, mapBuffSize) != mapBuffSize) {
		printf("RequestMap() - ReadDataTcp() is failed!\n");
		return;
	}

	// To prevent race condition
	algorithmmutex.unlock();

	// For debugging
	printf("RequestMap() - mapBuff: 0x");
	printSize = sizeof(unsigned char) + (sizeof(double) * 5);
	for (index = 0; index < printSize ; index++) {
		printf("%1x", mapBuff[index]);
	}
	printf("\n");

	if (mapBuff[0] != 0x2) {
		printf("RequestMap() - buff[0] is not 0x02. (%d)\n", mapBuff[0]);
		return;
	}

	std::memcpy(map, mapBuff+sizeof(unsigned char), MAP_SIZE);
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



//-----------------------------------------------------------------
// END AlgorithmController
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
