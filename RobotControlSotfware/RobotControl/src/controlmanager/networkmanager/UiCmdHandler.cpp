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

UiCmdHandler::UiCmdHandler(fp_ui_command fpCmd, fp_network_disconnected fpNw, fp_reset fpRe) {
	cmdBuffSize = sizeof(unsigned char) * 4;
	cmdBuff = (unsigned char *)malloc(cmdBuffSize);
	memset(cmdBuff, 0x00, cmdBuffSize);

	fpUiCommand = fpCmd;
	fpDisconnected = fpNw;
	fpReset = fpRe;
}

bool UiCmdHandler::Open(const char *portno) {
	printf("UiCmdHandler. Open port is %s.\n", portno);
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

//	CloseTcpListenPort(&TcpListenPort);

	if (cmdBuff != NULL) {
		free(cmdBuff);
		cmdBuff = NULL;
	}
}

void UiCmdHandler::HandleUiCmd() {
	// For debugging
	printf("HandleUiCmd() - UI command handle thread is running!!\n");

	do
	{
		if (TcpConnectedPort == NULL) {
			TcpConnectedPort = AcceptTcpConnection(TcpListenPort, &cli_addr, &clilen);
			if (TcpConnectedPort == NULL) {
				printf("HandleUiCmd() - AcceptTcpConnection Failed\n");
				Close();
				fpDisconnected();
				return;
			}
		}

		if (ReadDataTcp(TcpConnectedPort, cmdBuff, cmdBuffSize) != cmdBuffSize) {
			printf("HandleUiCmd() - ReadDataTcp is failed!\n");
			Close();
			fpDisconnected();
			return;
		}

//		printf("HandleUiCmd() - Received UI command.\n");

		ParseUiCmd();
	} while (1);
}

void UiCmdHandler::ParseUiCmd() {
	switch (cmdBuff[0]) {
	case 0x1:	// Command
		ParseMode();
		ParseDirection();
		ParseCamera();
		break;

	case 0x2:	// Reset
		Reset();
		break;

	default:
		printf("ParseUiCmd() - It's invalid command.(%d)\n", cmdBuff[0]);
		break;
	}
}

void UiCmdHandler::Reset() {
	switch (cmdBuff[1]) {
	case 0x1:
	case 0x2:
	case 0x3:
		fpReset(cmdBuff[1]);
	break;

	default:
		printf("UiCmdHandler Reset() - It's not supported algorithm.(%d)\n", cmdBuff[1]);
		break;
	}
}

void UiCmdHandler::ParseMode() {
	switch (cmdBuff[1]) {
	case 0x0:
		// Do nothing..
		break;

	case 0x1:	// Manual
		fpUiCommand(UI_COMMAND_MODE_MANUAL);
		printf("UI command transit manual mode.\n");
		break;

	case 0x2:	// Auto
		fpUiCommand(UI_COMMAND_MODE_AUTO);
		printf("UI command transit auto mode.\n");
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
		printf("UI command is forward.\n");
		break;

	case 0x2:	// Back
		fpUiCommand(UI_COMMAND_DIRECTION_BACK);
		printf("UI command is backward.\n");
		break;

	case 0x3:	// Left
		fpUiCommand(UI_COMMAND_DIRECTION_TURN_LEFT);
		printf("UI command is left.\n");
		break;

	case 0x4:	// Right
		fpUiCommand(UI_COMMAND_DIRECTION_TURN_RIGHT);
		printf("UI command is right.\n");
		break;

	case 0x5:	// Stop
		fpUiCommand(UI_COMMAND_DIRECTION_STOP);
		printf("UI command is stop.\n");
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
		printf("UI command is camera upper.\n");
		break;

	case 0x2:	// Lower
		fpUiCommand(UI_COMMAND_CAMERA_LOWER);
		printf("UI command is camera lower.\n");
		break;

	case 0x3:	// Left
		fpUiCommand(UI_COMMAND_CAMERA_LEFT);
		printf("UI command is camera left.\n");
		break;

	case 0x4:	// Right
		fpUiCommand(UI_COMMAND_CAMERA_RIGHT);
		printf("UI command is camera right.\n");
		break;

	case 0x5:	// Center
		fpUiCommand(UI_COMMAND_CAMERA_CENTER);
		printf("UI command is camera center.\n");
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
