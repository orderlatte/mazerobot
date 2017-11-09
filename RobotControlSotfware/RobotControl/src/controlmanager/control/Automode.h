/*
 * Automode.h
 *
 *  Created on: November 7, 2017
 *      Author: lg
 */

#ifndef AutomodeH
#define AutomodeH

#include <thread>		// For testing
#include "robot_operation.h"
#include "AlgorithmController.h"
#include "RobotPosition.h"
#include "WallFinder.h"

typedef enum
{
	AUTOMODE_STATUS_READY = 0,
	AUTOMODE_STATUS_WAITING_FOR_DIRECTION,
	AUTOMODE_STATUS_RECEIVED_MOVING_DIRECTION,
	AUTOMODE_STATUS_TURNING,
	AUTOMODE_STATUS_TURNED,
	AUTOMODE_STATUS_MOVING,
	AUTOMODE_STATUS_MOVED,
	AUTOMODE_STATUS_RECOGNIZING_SIGN,
	AUTOMODE_STATUS_MAX
} T_automode_status;


class Automode {

public:
	Automode(RobotPosition *position);
	void init();
	void setAlgorithmCtrl(AlgorithmController *algCtrl);
	void doOperation();
	fp_robot_turned getRobotTurnedFP();
	fp_robot_moved getRobotMovedFP();
	void stopRobot();
	void avoidLeftWall();
	void avoidRightWall();
	fp_ewsn_direction_result getEWSNDirectionFP();

private:
	void doReady();
	void doWaitingForDirection();
	void doReceivedMovingDirection();
	void doTurning();
	void doTurned();
	void doMoving();
	void doMoved();
	void doRecognizingSign();
	void sendRobotStatusToAlgorithm();
//	void getTestNextDirectionForTesting(WallFinder *wallData, fp_ewsn_direction callback);
//	void CallBabckToGetEWSNDirection(int ewsnDirection);
//	void CallBackRobotTurned();
//	void CallBackRobotMoved();
	void moveNextCell();
	void setStatus(T_automode_status status);
};


#endif /* AutomodeH */
