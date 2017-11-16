//------------------------------------------------------------------------------------------------
// File: UdpSendMap.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Send and receives OpenCV Mat Images in a UDP message commpressed as Jpeg images 
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <mutex>
#include "UdpSendMap.h"

using namespace std;

UdpSendMap::UdpSendMap(fp_getMap fp) {
	GetMap = fp;
}

int UdpSendMap::OpenUdp(const char *remotehostname, const char *remoteportno) {
	if ((UdpLocalPort=UdpMapSender.OpenUdpPort(0))==NULL)  // Open UDP Network port
	{
		printf("OpenUdpPort Failed\n");
		CloseUdp();
		return 0;
	}

	if  ((UdpDest=UdpMapSender.GetUdpDest(remotehostname,remoteportno))==NULL)  // Setup remote network destination to send images
	{
		printf("GetUdpDest Failed\n");
		CloseUdp();
		return 0;
	}

	Map = (unsigned char *)malloc(MAP_SIZE);
	memset(Map, 0x00, MAP_SIZE);

	// Start thread for sending map to Remote UI
	CreateThreadToSendMap();

	return 1;
}


void UdpSendMap::CloseUdp() {
	UdpMapSender.CloseUdpPort(&UdpLocalPort);
	UdpMapSender.DeleteUdpDest(&UdpDest);
	if (Map != NULL) {
		free((void *)Map);
		Map = NULL;
	}
	return;
}

void UdpSendMap::SendMap()
{
	// For debugging
	printf("Send map thread is running!!\n");

	while (1)
	{
		if (Map == NULL) {
			printf("Map is NULL!\n");
			continue;
		}

		GetMap(Map);

		if((UdpMapSender.SendUDPMsg(UdpLocalPort, UdpDest, Map, MAP_SIZE)) == -1) {
			printf("Error to send map to Remote UI");
		}

		// For debugging
//		printf("Send map!\n");

		sleep(1);
	}
}

void UdpSendMap::CreateThreadToSendMap(void)
{
	// Send map thread
	MapThread = new std::thread(&UdpSendMap::SendMap, this);
}



//-----------------------------------------------------------------
// END UdpRecvImageAsJpeg
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
