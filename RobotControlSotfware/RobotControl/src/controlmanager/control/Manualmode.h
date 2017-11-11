/*
 * Manualmode.h
 *
 *  Created on: November 7, 2017
 *      Author: lg
 */

#ifndef ManualmodeH
#define ManualmodeH

#include "robot_operation.h"
#include "RobotPosition.h"
#include "WallFinder.h"
#include "AlgorithmController.h"

typedef enum
{
	MANUALMODE_CMD_READY = 0,
	MANUALMODE_CMD_FORWARD,
	MANUALMODE_CMD_BACK,
	MANUALMODE_CMD_TURN_LEFT,
	MANUALMODE_CMD_TURN_RIGHT,
	MANUALMODE_CMD_STOP,
	MANUALMODE_CMD_CELL_MOVED,
	MANUALMODE_CMD_CAMERA_LEFT,
	MANUALMODE_CMD_CAMERA_RIGHT,
	MANUALMODE_CMD_CAMERA_UPPER,
	MANUALMODE_CMD_CAMERA_LOWER,
	MANUALMODE_CMD_CAMERA_CENTER,
	MANUALMODE_CMD_CAMERA_LEFT_WALL,
	MANUALMODE_CMD_CAMERA_RIGHT_WALL,
	MANUALMODE_CMD_MAX
} T_manualmode_command;


class Manualmode {

public:
	Manualmode(RobotPosition *position, AlgorithmController *algCtrl);
	void init();
	void doOperation();
//	fp_robot_turned getRobotTurnedFP();
//	fp_robot_moved getRobotMovedFP();
	void stopRobot();
	void avoidLeftWall();
	void avoidRightWall();
//	void setAlgorithmCtrl(AlgorithmController *algCtrl);
	void setCommand(T_manualmode_command command);
};


#endif /* AutomodeH */
