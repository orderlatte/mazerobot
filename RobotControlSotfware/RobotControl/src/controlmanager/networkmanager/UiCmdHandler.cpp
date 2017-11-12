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
#include "UiCmdHandler.h"
#include "NetworkTCP.h"

using namespace std;

static TTcpListenPort    *TcpListenPort = NULL;
static TTcpConnectedPort *TcpConnectedPort = NULL;
static struct sockaddr_in cli_addr;
static socklen_t          clilen = sizeof(cli_addr);
static unsigned char 	 *cmdBuff = NULL;
static int 				  cmdBuffSize;

//static std::mutex algorithmmutex;

UiCmdHandler::UiCmdHandler(fp_ui_command fp) {
	cmdBuffSize = sizeof(unsigned char) * 4;
	cmdBuff = (unsigned char *)malloc(cmdBuffSize);
	memset(cmdBuff, 0x00, cmdBuffSize);

	fpUiCommand = fp;
}

bool UiCmdHandler::Open(const char *portno) {
	TcpListenPort = OpenTcpListenPort(atoi(portno));
	if (TcpListenPort == NULL)  {
		printf("UiCmdHandler Open() - TcpListenPort is null!\n");
		return false;
	}

	CreateThreadToSendMap();

	return true;
}

void UiCmdHandler::Close() {
	if (TcpConnectedPort == NULL) {
		return;
	}

	if (TcpConnectedPort->ConnectedFd != BAD_SOCKET_FD) {
		CLOSE_SOCKET(TcpConnectedPort->ConnectedFd);
		TcpConnectedPort->ConnectedFd=BAD_SOCKET_FD;
	}

	delete TcpConnectedPort;
	TcpConnectedPort = NULL;

	CloseTcpListenPort(&TcpListenPort);

	if (cmdBuff != NULL) {
		free(cmdBuff);
		cmdBuff = NULL;
	}
}

void UiCmdHandler::HandleUiCmd() {
	// For debugging
	printf("HandleUiCmd() - UI command handle thread is running!!\n");

	while (1)
	{
		TcpConnectedPort = AcceptTcpConnection(TcpListenPort, &cli_addr, &clilen);
		if (TcpConnectedPort == NULL) {
			printf("HandleUiCmd() - AcceptTcpConnection Failed\n");
			// TODO: Suspend mode call
			return;
		}

		if (ReadDataTcp(TcpConnectedPort, cmdBuff, cmdBuffSize) != cmdBuffSize) {
			printf("HandleUiCmd() - ReadDataTcp is failed!\n");
			// TODO: Suspend mode call
			return;
		}

		printf("HandleUiCmd() - Received UI command.\n");

		ParseUiCmd();
	}
}

void UiCmdHandler::ParseUiCmd() {

	if (cmdBuff[0] != 0x1) {
		printf("ParseUiCmd() - It's invalid command.\n");
		return;
	}

	ParseMode();
	ParseDirection();
	ParseCamera();
}

void UiCmdHandler::ParseMode() {
	switch (cmdBuff[1]) {
	case 0x0:
		// Do nothing..
		break;

	case 0x1:	// Manual
		fpUiCommand(UI_COMMAND_MODE_MANUAL);
		break;

	case 0x2:	// Auto
		fpUiCommand(UI_COMMAND_MODE_AUTO);
		break;

	default:
		printf("ParseMode() - It's not supported mode.(%d)\n", cmdBuff[1]);
		break;
	}
}

void UiCmdHandler::ParseDirection() {
	switch (cmdBuff[2]) {
	case 0x0:
		// Do nothing..
		break;

	case 0x1:	// Forward
		fpUiCommand(UI_COMMAND_DIRECTION_FORWARD);
		break;

	case 0x2:	// Back
		fpUiCommand(UI_COMMAND_DIRECTION_BACK);
		break;

	case 0x3:	// Left
		fpUiCommand(UI_COMMAND_DIRECTION_TURN_LEFT);
		break;

	case 0x4:	// Right
		fpUiCommand(UI_COMMAND_DIRECTION_TURN_RIGHT);
		break;

	case 0x5:	// Stop
		fpUiCommand(UI_COMMAND_DIRECTION_STOP);
		break;

	default:
		printf("ParseDirection() - It's not supported mode.(%d)\n", cmdBuff[2]);
		break;
	}
}

void UiCmdHandler::ParseCamera() {
	switch (cmdBuff[3]) {
	case 0x0:
		// Do nothing..
		break;

	case 0x1:	// Upper
		fpUiCommand(UI_COMMAND_CAMERA_UPPER);
		break;

	case 0x2:	// Lower
		fpUiCommand(UI_COMMAND_CAMERA_LOWER);
		break;

	case 0x3:	// Left
		fpUiCommand(UI_COMMAND_CAMERA_LEFT);
		break;

	case 0x4:	// Right
		fpUiCommand(UI_COMMAND_CAMERA_RIGHT);
		break;

	case 0x5:	// Center
		fpUiCommand(UI_COMMAND_CAMERA_CENTER);
		break;

	default:
		printf("ParseCamera() - It's not supported mode.(%d)\n", cmdBuff[3]);
		break;
	}
}


void UiCmdHandler::CreateThreadToSendMap() {
	// Send map thread
	TcpThread = new std::thread(&UiCmdHandler::HandleUiCmd, this);
}




//-----------------------------------------------------------------
// END AlgorithmController
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
