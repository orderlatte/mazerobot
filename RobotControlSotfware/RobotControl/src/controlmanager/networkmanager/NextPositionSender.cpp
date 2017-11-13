//------------------------------------------------------------------------------------------------
// File: AlgorithmController.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
//------------------------------------------------------------------------------------------------
#include <NextPositionSender.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstring>
#include <mutex>
#include "NetworkTCP.h"

using namespace std;

static TTcpConnectedPort *TcpConnectedPort = NULL;
static unsigned char 	 *statusBuff = NULL;
static int 				  statusBuffSize;
static char     		 *Hostname = NULL;
static char              *Portno = NULL;

static std::mutex position_mutex;

NextPositionSender::NextPositionSender(char *hostname, char *portno) {
	statusBuffSize = sizeof(unsigned char) * 5;
	statusBuff = (unsigned char *)malloc(statusBuffSize);
	memset(statusBuff, 0x00, statusBuffSize);

	Hostname = hostname;
	Portno = portno;

	Init();
}

void NextPositionSender::Init() {
	PositionX = 5;
	PositionY = 5;
}


bool NextPositionSender::Open() {
	TcpConnectedPort = OpenTcpConnection((const char *)Hostname, (const char *)Portno);

	if (TcpConnectedPort == NULL) {
		printf("OpenTcpConnection\n");
		return false;
	}

	// Send map thread
	TcpThread = new std::thread(&NextPositionSender::SendThread, this);

	return true;
}

void NextPositionSender::Close() {
	if (TcpConnectedPort == NULL) {
		return;
	}

	if (TcpConnectedPort->ConnectedFd != BAD_SOCKET_FD) {
		CLOSE_SOCKET(TcpConnectedPort->ConnectedFd);
		TcpConnectedPort->ConnectedFd=BAD_SOCKET_FD;
	}

	delete TcpConnectedPort;
	TcpConnectedPort = NULL;

	if (statusBuff != NULL) {
		free(statusBuff);
		statusBuff = NULL;
	}
}

void NextPositionSender::SendThread() {
	short *tmpPosition = 0x0;

	printf("SendThread() - Next Position thread is running!!\n");

	statusBuff[0] = 0x3;

	while (1) {
		// For debugging

		position_mutex.lock();

		tmpPosition = (short *)&statusBuff[2];
		*tmpPosition = (short)PositionX;

		tmpPosition = (short *)&statusBuff[4];
		*tmpPosition = (short)PositionY;

		position_mutex.unlock();

		printf("SendThread() Next position : %d, %d\n", PositionX, PositionY);

		if (TcpConnectedPort == NULL) {
			TcpConnectedPort = OpenTcpConnection((const char *)Hostname, (const char *)Portno);
		}

		if (TcpConnectedPort == NULL) {
			printf("NextPositionSender::SendThread() - After WriteDataTcp() - Open() is failed!\n");
			usleep(500000); // 500 miliseconds
			continue;
		}

		if (WriteDataTcp(TcpConnectedPort,statusBuff,statusBuffSize) != sizeof(statusBuffSize)) {
			printf("NextPositionSender::SendThread() - WriteDataTcp() is failed!\n");
		}

		usleep(500000); // 500 miliseconds
	}
}

void NextPositionSender::SendPosition(int positionX, int positionY) {
	position_mutex.lock();

	PositionX = positionX;
	PositionY = positionY;

	position_mutex.unlock();
}


//-----------------------------------------------------------------
// END AlgorithmController
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
