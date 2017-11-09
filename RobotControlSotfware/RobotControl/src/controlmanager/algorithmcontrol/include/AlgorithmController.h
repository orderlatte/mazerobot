/*
 * AlgorithmController.h
 *
 *  Created on: Oct 30, 2017
 *      Author: lg
 */

#ifndef AlgorithmControllerH
#define AlgorithmControllerH

#include <thread>
#include "NetworkTCP.h"
#include "UdpSendMap.h"
#include "RobotPosition.h"
#include "WallFinder.h"

typedef enum
{
	ROBOT_COMMAND_MAP_REQUEST = 1,
	ROBOT_COMMAND_INFORMATION_SEND =  4
} T_robot_command;

// 1: EAST, 2: WEST, 4: SOUTH, 8: NORTH (first parameter)
// 1: Success, 2: Error
typedef void (*fp_ewsn_direction_result)(int, int);

class AlgorithmController {

private:
	fp_ewsn_direction_result fpEWSNDirectionResult = NULL;
//	UdpSendMap		  *SendMap = NULL;
	std::thread	      *TcpThread = NULL;

public:
	AlgorithmController(fp_ewsn_direction_result fp);
	void SendRobotCell(RobotPosition *robotPosition, int signPosition, int signType, int redDot, WallFinder *wall);
	void Open();
	void Close();
	fp_getMap GetMapFP();

private:
	int GetNextDirection(unsigned char direction);
	int GetResultFromAlgorithm(unsigned char result);
	void SendRobotCellThread(RobotPosition *robotPosition, int signPosition, int signType, int redDot, WallFinder *wall);
};


#endif /* AlgorithmControllerH */
