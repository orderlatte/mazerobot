//------------------------------------------------------------------------------------------------
// File: UdpSendJpeg.cpp
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
#include "UdpSendJpeg.h"

using namespace std;

static int init_values[2] = { cv::IMWRITE_JPEG_QUALITY,80 }; //default(95) 0-100
static std::vector<int> param (&init_values[0], &init_values[0]+2);
static std::vector<uchar> sendbuff;//buffer for coding
static std::mutex jpegmutex;
static bool isSetImage = false; // false: Image is not set. true: Image is set.

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

	CreateThreadToSendJpeg();

	return 1;
}

void UdpSendJpeg::SetImage(cv::Mat *image)
{
	jpegmutex.lock();
	cv::imencode(".jpg", *image, sendbuff, param);
	jpegmutex.unlock();

	isSetImage = true;
	return;
}

void UdpSendJpeg::CloseUdp() {
	UdpSender.CloseUdpPort(&UdpLocalPort);
	UdpSender.DeleteUdpDest(&UdpDest);
	isSetImage = false;
	return;
}


void UdpSendJpeg::SendJpeg()
{
	// For debugging
	printf("Send map thread is running!!\n");

	while (1)
	{
		usleep(10000);	// sleep 10 milliseconds

		if (isSetImage == false) {
			continue;
		}

		jpegmutex.lock();
		if (UdpSender.SendUDPMsg(UdpLocalPort,UdpDest,sendbuff.data(), sendbuff.size()) == -1) {
			printf("UdpSenderJpeg -> SendUdpMsg error!");
			continue;
		}
		jpegmutex.unlock();
	}
}

void UdpSendJpeg::CreateThreadToSendJpeg(void)
{
	// Send map thread
	JpegThread = new std::thread(&UdpSendJpeg::SendJpeg, this);
}
//-----------------------------------------------------------------
// END UdpRecvImageAsJpeg
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
