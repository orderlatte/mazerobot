/*
 * UdpSendJpeg.h
 *
 *  Created on: Oct 30, 2017
 *      Author: lg
 */

#ifndef UDPSendMapH
#define UDPSendMapH

#include <thread>
#include "NetworkUDP.h"

class UdpSendMap {

private:
	TUdpLocalPort *UdpLocalPort=NULL;
	TUdpDest      *UdpDest=NULL;
	NetworkUDP     UdpMapSender;
	unsigned char *Map=NULL;
	int			   MapSize=0;
	std::thread	  *MapThread=NULL;

public:
	int OpenUdp(const char *remotehostname, const char *remoteportno);
	void SetMap(unsigned char *map);
	void CloseUdp();

private:
	void SendMap();
	void CreateThreadToSendMap(void);
};


#endif /* UDPSendMapH */
