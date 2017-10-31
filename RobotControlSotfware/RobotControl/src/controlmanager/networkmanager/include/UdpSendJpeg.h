/*
 * UdpSendJpeg.h
 *
 *  Created on: Oct 30, 2017
 *      Author: lg
 */

#ifndef UDPSendJPEGH
#define UDPSendJPEGH

#include <opencv2/highgui/highgui.hpp>
#include "NetworkUDP.h"

class UdpSendJpeg {

private:
	TUdpLocalPort *UdpLocalPort=NULL;
	TUdpDest      *UdpDest=NULL;
	NetworkUDP     UdpSender;

public:
	int OpenUdp(const char *remotehostname, const char *remoteportno);
	int UdpSendImageAsJpeg(cv::Mat Image);
	void CloseUdp();
};


#endif /* UDPSendJPEGH */
