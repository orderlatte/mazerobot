/*
 * UdpSendJpeg.h
 *
 *  Created on: Oct 30, 2017
 *      Author: lg
 */

#ifndef UDPSendJPEGH
#define UDPSendJPEGH

#include <thread>
#include <opencv2/highgui/highgui.hpp>
#include "NetworkUDP.h"

class UdpSendJpeg {

private:
	TUdpLocalPort *UdpLocalPort=NULL;
	TUdpDest      *UdpDest=NULL;
	NetworkUDP     UdpSender;
	std::thread	  *JpegThread=NULL;

public:
	int OpenUdp(const char *remotehostname, const char *remoteportno);
	void SetImage(cv::Mat *image);
	void CloseUdp();


private:
	void SendJpeg();
	void CreateThreadToSendJpeg(void);
};


#endif /* UDPSendJPEGH */
