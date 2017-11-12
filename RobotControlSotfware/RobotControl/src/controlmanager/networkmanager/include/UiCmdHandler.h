/*
 * UiCmdHandler.h
 *
 *  Created on: Oct 30, 2017
 *      Author: lg
 */

#ifndef UiCmdHandlerH
#define UiCmdHandlerH

#include <thread>
#include "NetworkTCP.h"

typedef enum
{
	UI_COMMAND_MODE_MANUAL = 0,
	UI_COMMAND_MODE_AUTO,
	UI_COMMAND_DIRECTION_FORWARD,
	UI_COMMAND_DIRECTION_BACK,
	UI_COMMAND_DIRECTION_TURN_LEFT,
	UI_COMMAND_DIRECTION_TURN_RIGHT,
	UI_COMMAND_DIRECTION_STOP,
	UI_COMMAND_CELL_MOVED,
	UI_COMMAND_CAMERA_LEFT,
	UI_COMMAND_CAMERA_RIGHT,
	UI_COMMAND_CAMERA_UPPER,
	UI_COMMAND_CAMERA_LOWER,
	UI_COMMAND_CAMERA_CENTER,
	UI_COMMAND_CAMERA_LEFT_WALL,
	UI_COMMAND_CAMERA_RIGHT_WALL,
	UI_COMMAND_MAX
} T_ui_command;

typedef void (*fp_ui_command)(T_ui_command command);


class UiCmdHandler {

private:
	std::thread *TcpThread = NULL;
	fp_ui_command fpUiCommand;

public:
	UiCmdHandler(fp_ui_command fp);
	bool Open(const char *portno);
	void Close();

private:
	void HandleUiCmd();
	void ParseUiCmd();
	void ParseMode();
	void ParseDirection();
	void ParseCamera();
	void CreateThreadToSendMap();
};


#endif /* UiCmdHandlerH */
