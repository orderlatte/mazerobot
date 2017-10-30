//------------------------------------------------------------------------------------------------
// File: LineFollower.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Main line follower Program. This is the main program follows a line. 
// To invoke LineFollower <hostname> <port>. This program will send the processed camera image 
// via UDP to the RecvImage_UDP program that will display the image
// 
// Use the following keys
// r key - run mode (starts the robot following the line
// s key - disables run mode and stops the robot
// j key - moves the camera pan servo left
// l key - moves the camera pan servo right
// i key - moves the camera tilt servo up
// m key - moves the camera tilt servo down
// k key - moves the camera pan and tilt servos to center
// q key - descreases the speed of the robot (manual control only both wheels move at the same speed)
// w key - stops the robot (manual control only)
// e key - increases the speed of the robot (manual control only both wheels move at the same speed)
//----------------------------------------------------------------------------------------------
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <iostream>

#include "ImageProcessing.h"
#include "PI3OpencvCompat.h"
#include "PID.h"
#include "Servos.h"
#include "NetworkUDP.h"
#include "UdpSendJpeg.h"
#include "KeyboardSetup.h"

#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.
#include <wiringPi.h>
#endif //UBUNTU

#include "sensor_manager.h"
#include "servos_manager.h"



#define INIT 0
#define STOP 1
#define FOWARD 2
#define LEFT 3
#define RIGHT 4







using namespace cv;
using namespace std;


#define WIDTH 640
#define HEIGHT 480



static int            AWidth;
static int            AHeight;
static int            Pan;
static int            Tilt;
static CvCapture *    capture=NULL;
static UdpSendJpeg    VideoSender;
static TPID           PID;  
static int           Run=INIT;

float	   offset;		   // computed robot deviation from the line 






static void  Setup_Control_C_Signal_Handler_And_Keyboard_No_Enter(void);
static void  CleanUp(void);
static void  Control_C_Handler(int s);
static void  HandleInputChar(void);

//----------------------------------------------------------------
// main - This is the main program for the line follower and 
// contains the control loop
//-----------------------------------------------------------------
int main(int argc, const char** argv)
{
  IplImage * iplCameraImage; // camera image in IplImage format 
  Mat        image;          // camera image in Mat format 



   if (argc !=3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

  Setup_Control_C_Signal_Handler_And_Keyboard_No_Enter(); // Set Control-c handler to properly exit cleanly 




  if (VideoSender.OpenUdp(argv[1],argv[2]) == 0) // Setup remote network destination to send images
  {
	  printf("OpenUdpPort Failed\n");
	  CleanUp();
	  return(-1);
  }

  capture =cvCreateCameraCapture(0);   // Open default Camera
    if(!capture)
      {
        printf("Camera Not Initialized\n");
        CleanUp();
        return 0;
      }

  if (cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,WIDTH)==0) // Set camera width 
    {
      printf("cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,WIDTH) Failed)\n");
    }

 if (cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,HEIGHT)==0) // Set camera height
    {
      printf("cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,HEIGHT) Failed)\n");
    }

  AWidth=cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
  AHeight=cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);

  printf("Width = %d\n",AWidth );
  printf("Height = %d\n", AHeight);

  if (!IsPi3) namedWindow( "camera", CV_WINDOW_AUTOSIZE ); // If not running on PI3 open local Window
 
 //if (!IsPi3) namedWindow( "processed", CV_WINDOW_AUTOSIZE );  // If not running on PI3 open local Window
  do
   {
	sensor_manager_main();
	servos_manager_main();

    iplCameraImage = cvQueryFrame(capture); // Get Camera image
    image= cv::cvarrToMat(iplCameraImage);  // Convert Camera image to Mat format
    if (IsPi3) flip(image, image,-1);       // if running on PI3 flip(-1)=180 degrees

    offset=FindLineInImageAndComputeOffset(image); // Process camera image / locat line and compute offset from line

	VideoSender.UdpSendImageAsJpeg(image);
  
    if (!IsPi3) imshow("camera", image );             // Show image locally if not running on PI 3
    HandleInputChar();                                // Handle Keyboard Input
    if (!IsPi3) cv::waitKey(1);                       // must be call show image locally work with imshow

  } while (1);


  return 0;
}
//-----------------------------------------------------------------
// END main
//-----------------------------------------------------------------
//----------------------------------------------------------------
// Setup_Control_C_Signal_Handler_And_Keyboard_No_Enter - This 
// sets uo the Control-c Handler and put the keyboard in a mode
// where it will not
// 1. echo input
// 2. need enter hit to get a character 
// 3. block waiting for input
//-----------------------------------------------------------------
static void Setup_Control_C_Signal_Handler_And_Keyboard_No_Enter(void)
{
 struct sigaction sigIntHandler;
 sigIntHandler.sa_handler = Control_C_Handler; // Setup control-c callback 
 sigemptyset(&sigIntHandler.sa_mask);
 sigIntHandler.sa_flags = 0;
 sigaction(SIGINT, &sigIntHandler, NULL);
 ConfigKeyboardNoEnterBlockEcho();             // set keyboard configuration
}
//-----------------------------------------------------------------
// END Setup_Control_C_Signal_Handler_And_Keyboard_No_Enter
//-----------------------------------------------------------------
//----------------------------------------------------------------
// CleanUp - Performs cleanup processing before exiting the
// the program
//-----------------------------------------------------------------
static void CleanUp(void)
{
 RestoreKeyboard();                // restore Keyboard
 
  if (capture!=NULL)  
      {
       cvReleaseCapture(&capture); // Close camera
       capture=NULL;
      }

 VideoSender.CloseUdp();
 ResetServos();                    // Reset servos to center or stopped
 CloseServos();                    // Close servo device driver
 printf("restored\n");
}
//-----------------------------------------------------------------
// END CleanUp
//-----------------------------------------------------------------
//----------------------------------------------------------------
// Control_C_Handler - called when control-c pressed
//-----------------------------------------------------------------
static void Control_C_Handler(int s)
{
 CleanUp();
 printf("Caught signal %d\n",s);
 printf("exiting\n");
 exit(1);
}
//-----------------------------------------------------------------
// END Control_C_Handler
//-----------------------------------------------------------------
//----------------------------------------------------------------
// HandleInputChar - check if keys are press and proccess keys of
// interest.
//-----------------------------------------------------------------
static void HandleInputChar(void)
{
 int ch;
 static int speed=0;
  if ((ch=getchar())==EOF)              // no key pressed return
     {
       return;
     }

	if  (ch=='w')
	{
		robot_mode_setting(ROBOT_FOWARD_MOVING,offset);
    }
	else if  (ch=='x')
    {
		robot_mode_setting(ROBOT_BACKWARD_MOVING,offset);
    }
	else if  (ch=='d')
    {
		robot_mode_setting(ROBOT_RIGHT_ROTATING,offset);
    }
	else if  (ch=='a')
    {
		robot_mode_setting(ROBOT_LEFT_ROTATING,offset);
    }
	else
	{
		robot_mode_setting(ROBOT_STOP,offset);
	}
	

}

//-----------------------------------------------------------------
// END HandleInputChar
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------

