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

class NextPositionSender {

private:
	std::thread *TcpThread = NULL;
	int PositionX;
	int PositionY;

public:
	NextPositionSender(char *hostname, char *portno);
	void Init();
	bool Open();
	void Close();
	void SendPosition(int positionX, int positionY);

private:
	void SendThread();
};


#endif /* StatusSenderH */
