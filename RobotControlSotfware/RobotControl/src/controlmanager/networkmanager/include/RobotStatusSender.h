/*
 * StatusSender.h
 *
 *  Created on: Oct 30, 2017
 *      Author: lg
 */

#ifndef StatusSenderH
#define StatusSenderH

#include <thread>
#include "NetworkTCP.h"

class RobotStatusSender {

private:
	std::thread *TcpThread = NULL;
	std::thread *ModeThread = NULL;
	int PositionX;
	int PositionY;
	int PreviousPositionX;
	int PreviousPorisionY;
	int RobotMode;
	int PreviousRobotMode;

public:
	RobotStatusSender(char *hostname, char *portno, int mode);
	void Init();
	bool Open();
	void Close();
	void SendPosition(int positionX, int positionY);
	void SendMode(int mode);
	void SendFullyMap();

private:
	void SendNextPositionThread();
	void SendRobotModeThread();
};


#endif /* StatusSenderH */
