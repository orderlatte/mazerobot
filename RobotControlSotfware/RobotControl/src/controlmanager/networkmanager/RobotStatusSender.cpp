//------------------------------------------------------------------------------------------------
// File: AlgorithmController.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
//------------------------------------------------------------------------------------------------
#include <RobotStatusSender.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstring>
#include <mutex>
#include "NetworkTCP.h"

using namespace std;

static TTcpConnectedPort *TcpConnectedPort = NULL;
static unsigned char 	 *positionBuff = NULL;
static int 				  positionBuffSize;
static unsigned char     *modeBuff = NULL;
static int			      modeBuffSize;
static unsigned char     *fullyMapBuff = NULL;
static int				  fullyMapBuffSize;
static char     		 *Hostname = NULL;
static char              *Portno = NULL;

//static std::mutex position_mutex;
//static std::mutex mode_mutex;
static std::mutex sender_mutex;

RobotStatusSender::RobotStatusSender(char *hostname, char *portno, int mode) {
	positionBuffSize = sizeof(unsigned char) * 5;
	positionBuff = (unsigned char *)malloc(positionBuffSize);
	memset(positionBuff, 0x00, positionBuffSize);

	modeBuffSize = sizeof(unsigned char) * 2;
	modeBuff = (unsigned char *)malloc(modeBuffSize);
	memset(modeBuff, 0x00, modeBuffSize);

	fullyMapBuffSize = sizeof(unsigned char) * 2;
	fullyMapBuff = (unsigned char *)malloc(fullyMapBuffSize);
	memset(fullyMapBuff, 0x00, fullyMapBuffSize);

	Hostname = hostname;
	Portno = portno;

	RobotMode = mode + 1;
	PreviousRobotMode = RobotMode;

	Init();
}

void RobotStatusSender::Init() {
	PositionX = 5;
	PositionY = 5;
	PreviousPositionX = 5;
	PreviousPorisionY = 5;
}


bool RobotStatusSender::Open() {
	TcpConnectedPort = OpenTcpConnection((const char *)Hostname, (const char *)Portno);

	if (TcpConnectedPort == NULL) {
		printf("OpenTcpConnection\n");
		return false;
	}

	// Send map thread
	TcpThread = new std::thread(&RobotStatusSender::SendNextPositionThread, this);
	ModeThread = new std::thread(&RobotStatusSender::SendRobotModeThread, this);

	return true;
}

void RobotStatusSender::Close() {
	if (TcpConnectedPort == NULL) {
		return;
	}

	if (TcpConnectedPort->ConnectedFd != BAD_SOCKET_FD) {
		CLOSE_SOCKET(TcpConnectedPort->ConnectedFd);
		TcpConnectedPort->ConnectedFd=BAD_SOCKET_FD;
	}

	delete TcpConnectedPort;
	TcpConnectedPort = NULL;

	if (positionBuff != NULL) {
		free(positionBuff);
		positionBuff = NULL;
	}
}

void RobotStatusSender::SendNextPositionThread() {
	int index = 0;
	short *tmpPosition = 0x0;

	printf("SendNextPositionThread() - Next Position thread is running!!\n");

	while (1) {
		positionBuff[0] = 0x3;

//		position_mutex.lock();

		tmpPosition = (short *)&positionBuff[2];
		*tmpPosition = (short)PositionX;

		tmpPosition = (short *)&positionBuff[4];
		*tmpPosition = (short)PositionY;

		if ((PreviousPositionX != PositionX) || (PreviousPorisionY != PositionY)) {
			// For debugging
			printf("SendNextPositionThread() - positionBuff: 0x");
			for (index = 0; index < positionBuffSize; index++) {
				printf("%1x", positionBuff[index]);
			}
			printf("\n");

			PreviousPositionX = PositionX;
			PreviousPorisionY = PositionY;
//			position_mutex.unlock();

		} else {
			usleep(250000); // 250 miliseconds

//			position_mutex.unlock();
			continue;
		}


		sender_mutex.lock();

		if (TcpConnectedPort == NULL) {
			TcpConnectedPort = OpenTcpConnection((const char *)Hostname, (const char *)Portno);
		}

		if (TcpConnectedPort == NULL) {
			printf("SendNextPositionThread() - After WriteDataTcp() - Open() is failed!\n");
			goto bailout;
		}

		if (WriteDataTcp(TcpConnectedPort,positionBuff,positionBuffSize) != positionBuffSize) {
			printf("SendNextPositionThread() - WriteDataTcp() is failed!\n");
			goto bailout;
		}

bailout:

		sender_mutex.unlock();

		usleep(250000); // 250 miliseconds
	}
}

void RobotStatusSender::SendPosition(int positionX, int positionY) {
//	position_mutex.lock();

	PositionX = positionX;
	PositionY = positionY;

	printf("SendPosition() Next position : %d, %d\n", PositionX, PositionY);

//	position_mutex.unlock();
}

void RobotStatusSender::SendRobotModeThread() {
	int index = 0;
	printf("SendRobotModeThread() - Next Position thread is running!!\n");


	while (1) {

		modeBuff[0] = 0x5;

//		mode_mutex.lock();

		modeBuff[1] = (unsigned char) RobotMode;

		if (PreviousRobotMode != RobotMode) {
			// For debugging
			printf("SendRobotModeThread() - modeBuff: 0x");
			for (index = 0; index < modeBuffSize; index++) {
				printf("%1x", modeBuff[index]);
			}
			printf("\n");

			PreviousRobotMode = RobotMode;

//			mode_mutex.unlock();
		} else {
//			mode_mutex.unlock();

			usleep(250000); // 250 miliseconds
			continue;
		}



		sender_mutex.lock();

		if (TcpConnectedPort == NULL) {
			TcpConnectedPort = OpenTcpConnection((const char *)Hostname, (const char *)Portno);
		}

		if (TcpConnectedPort == NULL) {
			printf("SendRobotModeThread() - After WriteDataTcp() - Open() is failed!\n");
			goto bailout;
		}

		if (WriteDataTcp(TcpConnectedPort,modeBuff,modeBuffSize) != modeBuffSize) {
			printf("SendRobotModeThread() - WriteDataTcp() is failed!\n");
			goto bailout;
		}

bailout:
		sender_mutex.unlock();

		usleep(250000); // 250 miliseconds
	}
}

void RobotStatusSender::SendMode(int mode) {
//	mode_mutex.lock();

	RobotMode = mode + 1;

//	mode_mutex.unlock();
}

void RobotStatusSender::SendFullyMap() {

	printf("SendFullyMap() is called.\n");

	fullyMapBuff[0] = 0x6;
	fullyMapBuff[1] = 0x1;

	sender_mutex.lock();

	if (TcpConnectedPort == NULL) {
		TcpConnectedPort = OpenTcpConnection((const char *)Hostname, (const char *)Portno);
	}

	if (TcpConnectedPort == NULL) {
		printf("SendFullyMap() - OpenTcpConnection() is failed!\n");
		goto bailout;
	}

	if (WriteDataTcp(TcpConnectedPort,fullyMapBuff,fullyMapBuffSize) != fullyMapBuffSize) {
		printf("SendFullyMap() - WriteDataTcp() is failed!\n");
		goto bailout;
	}

bailout:

	sender_mutex.unlock();
}

//-----------------------------------------------------------------
// END AlgorithmController
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
