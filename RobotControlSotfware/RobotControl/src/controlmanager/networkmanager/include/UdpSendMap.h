/*
 * UdpSendMap.h
 *
 *  Created on: Oct 30, 2017
 *      Author: lg
 */

#ifndef UDPSendMapH
#define UDPSendMapH

#include <thread>
#include "NetworkUDP.h"

#define MAP_SIZE (11*11*sizeof(double))

typedef void (*fp_getMap)(unsigned char *map);

class UdpSendMap {

private:
	TUdpLocalPort *UdpLocalPort = NULL;
	TUdpDest      *UdpDest = NULL;
	NetworkUDP     UdpMapSender;
	unsigned char *Map = NULL;
	std::thread	  *MapThread = NULL;
	fp_getMap      GetMap = NULL;

public:
	UdpSendMap(fp_getMap fp);
	int OpenUdp(const char *remotehostname, const char *remoteportno);
//	void SetMap(unsigned char *map);
	void CloseUdp();

private:
	void SendMap();
	void CreateThreadToSendMap(void);
};


#endif /* UDPSendMapH */
