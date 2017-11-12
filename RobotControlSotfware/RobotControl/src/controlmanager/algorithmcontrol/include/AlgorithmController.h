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
#include "FloorFinder.h"
#include "StartingPoint.h"

typedef enum
{
	ROBOT_COMMAND_MAP_REQUEST = 1,
	ROBOT_COMMAND_INFORMATION_SEND =  4
} T_robot_command;

typedef enum
{
	ALGORITHM_RESULT_OK = 0,
	ALGORITHM_RESULT_FULLY_MAPPED,
	ALGORITHM_RESULT_ERROR
} T_algorithm_result;

// 1: EAST, 2: WEST, 4: SOUTH, 8: NORTH (first parameter)
// 1: Success, 2: Error
typedef void (*fp_ewsn_direction_result)(int, T_algorithm_result);

class AlgorithmController {

private:
	fp_ewsn_direction_result fpEWSNDirectionResult = NULL;
//	UdpSendMap		  *SendMap = NULL;
	std::thread	      *TcpThread = NULL;

public:
	AlgorithmController(StartingPoint *starting);
	void SendRobotCell(RobotPosition *robotPosition, int signPosition, int signType, FloorFinder *floor, WallFinder *wall, fp_ewsn_direction_result fp);
	void Reset();
	bool Open();
	void Close();
	fp_getMap GetMapFP();

private:
	int GetNextDirection(unsigned char direction);
	T_algorithm_result GetResultFromAlgorithm(unsigned char result);
	void SendRobotCellThread(RobotPosition *robotPosition, int signPosition, int signType, FloorFinder *floor, WallFinder *wall, fp_ewsn_direction_result fp);
};


#endif /* AlgorithmControllerH */
