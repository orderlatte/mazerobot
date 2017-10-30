//------------------------------------------------------------------------------------------------
// File: UdpSendRecvJpeg.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Send and receives OpenCV Mat Images in a UDP message commpressed as Jpeg images 
//------------------------------------------------------------------------------------------------
#include "UdpSendJpeg.h"

static int init_values[2] = { cv::IMWRITE_JPEG_QUALITY,80 }; //default(95) 0-100
static std::vector<int> param (&init_values[0], &init_values[0]+2);
static std::vector<uchar> sendbuff;//buffer for coding

int UdpSendJpeg::OpenUdp(const char *remotehostname, const char *remoteportno) {
	if ((UdpLocalPort=UdpSender.OpenUdpPort(0))==NULL)  // Open UDP Network port
	{
		printf("OpenUdpPort Failed\n");
		CloseUdp();
		return 0;
	}

	if  ((UdpDest=UdpSender.GetUdpDest(remotehostname,remoteportno))==NULL)  // Setup remote network destination to send images
	{
		printf("GetUdpDest Failed\n");
		CloseUdp();
		return 0;
	}

	return 1;
}

//-----------------------------------------------------------------
// UdpSendImageAsJpeg - Sends a Open CV Mat Image commressed as a 
// jpeg image in side a UDP Message on the specified UDP local port
// and Destination. return bytes sent on success and -1 on failure
//-----------------------------------------------------------------
int UdpSendJpeg::UdpSendImageAsJpeg(cv::Mat Image)
{
    cv::imencode(".jpg", Image, sendbuff, param);
    return(UdpSender.SendUDPMsg(UdpLocalPort,UdpDest,sendbuff.data(), sendbuff.size()));
}

void UdpSendJpeg::CloseUdp() {
	UdpSender.CloseUdpPort(&UdpLocalPort);
	UdpSender.DeleteUdpDest(&UdpDest);
	return;
}

//-----------------------------------------------------------------
// END UdpRecvImageAsJpeg
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
