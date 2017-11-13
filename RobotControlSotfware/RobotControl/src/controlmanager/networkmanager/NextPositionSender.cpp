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

//static std::mutex algorithmmutex;

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

	if (TcpConnectedPort == NULL) {
		if (Open() == false) {
			printf("NextPositionSender::SendThread() - Open() is failed!\n");
			return;
		}
	}

	// For debugging
	printf("SendThread() - Next Position thread is running!!\n");

	statusBuff[0] = 0x3;

	tmpPosition = (short *)&statusBuff[2];
	*tmpPosition = (short)PositionX;

	tmpPosition = (short *)&statusBuff[4];
	*tmpPosition = (short)PositionY;

	printf("SendThread() Next position : %d, %d\n", PositionX, PositionY);

	if (WriteDataTcp(TcpConnectedPort,statusBuff,statusBuffSize) != sizeof(statusBuffSize)) {
		printf("SendThread() is failed!\n");
		return;
	}
}

void NextPositionSender::SendPosition(int positionX, int positionY) {
	PositionX = positionX;
	PositionY = positionY;

	// Send map thread
	TcpThread = new std::thread(&NextPositionSender::SendThread, this);
}


//-----------------------------------------------------------------
// END AlgorithmController
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
