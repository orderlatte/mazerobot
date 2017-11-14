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
static char     		 *Hostname = NULL;
static char              *Portno = NULL;
static int				 *ModeStatus = NULL;

static std::mutex position_mutex;
static std::mutex mode_mutex;
static std::mutex sender_mutex;

RobotStatusSender::RobotStatusSender(char *hostname, char *portno, int mode) {
	positionBuffSize = sizeof(unsigned char) * 5;
	positionBuff = (unsigned char *)malloc(positionBuffSize);
	memset(positionBuff, 0x00, positionBuffSize);

	modeBuffSize = sizeof(unsigned char) * 2;
	modeBuff = (unsigned char *)malloc(modeBuffSize);
	memset(modeBuff, 0x00, modeBuffSize);

	Hostname = hostname;
	Portno = portno;

	RobotMode = mode;

	Init();
}

void RobotStatusSender::Init() {
	PositionX = 5;
	PositionY = 5;
}


bool RobotStatusSender::Open() {
	TcpConnectedPort = OpenTcpConnection((const char *)Hostname, (const char *)Portno);

	if (TcpConnectedPort == NULL) {
		printf("OpenTcpConnection\n");
		return false;
	}

	// Send map thread
	TcpThread = new std::thread(&RobotStatusSender::SendNextPositionThread, this);

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
	short *tmpPosition = 0x0;

	printf("SendNextPositionThread() - Next Position thread is running!!\n");

	positionBuff[0] = 0x3;

	while (1) {
		// For debugging

		position_mutex.lock();

		tmpPosition = (short *)&positionBuff[2];
		*tmpPosition = (short)PositionX;

		tmpPosition = (short *)&positionBuff[4];
		*tmpPosition = (short)PositionY;

		position_mutex.unlock();


		printf("SendNextPositionThread() Next position : %d, %d\n", PositionX, PositionY);

		sender_mutex.lock();

		if (TcpConnectedPort == NULL) {
			TcpConnectedPort = OpenTcpConnection((const char *)Hostname, (const char *)Portno);
		}

		if (TcpConnectedPort == NULL) {
			printf("SendNextPositionThread() - After WriteDataTcp() - Open() is failed!\n");
			goto bailout;
		}

		if (WriteDataTcp(TcpConnectedPort,positionBuff,positionBuffSize) != sizeof(positionBuffSize)) {
			printf("SendNextPositionThread() - WriteDataTcp() is failed!\n");
		}

bailout:

		sender_mutex.unlock();

		usleep(250); // 250 miliseconds
	}
}

void RobotStatusSender::SendPosition(int positionX, int positionY) {
	position_mutex.lock();

	PositionX = positionX;
	PositionY = positionY;

	position_mutex.unlock();
}

void RobotStatusSender::SendRobotModeThread() {

	printf("SendRobotModeThread() - Next Position thread is running!!\n");

	modeBuff[0] = 0x5;

	while (1) {
		// For debugging

		mode_mutex.lock();

		modeBuff[1] = (unsigned char) RobotMode;

		mode_mutex.unlock();


		sender_mutex.lock();

		printf("SendRobotModeThread() Next position : %d, %d\n", PositionX, PositionY);

		if (TcpConnectedPort == NULL) {
			TcpConnectedPort = OpenTcpConnection((const char *)Hostname, (const char *)Portno);
		}

		if (TcpConnectedPort == NULL) {
			printf("SendRobotModeThread() - After WriteDataTcp() - Open() is failed!\n");
			goto bailout;
		}

		if (WriteDataTcp(TcpConnectedPort,modeBuff,modeBuffSize) != sizeof(modeBuffSize)) {
			printf("SendRobotModeThread() - WriteDataTcp() is failed!\n");
		}

bailout:
		sender_mutex.unlock();

		usleep(250); // 300 miliseconds
	}
}

void RobotStatusSender::SendMode(int mode) {
	mode_mutex.lock();

	RobotMode = mode;

	mode_mutex.unlock();
}

//-----------------------------------------------------------------
// END AlgorithmController
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
