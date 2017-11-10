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
#include <stdio.h>
#include <signal.h>
#include <iostream>

#include "UdpSendMap.h"
#include "KeyboardSetup.h"
#include "RobotVisionManager.h"
#include "robot_operation.h"
#include "UdpSendJpeg.h"
#include "Direction.h"
#include "sensor_manager.h"
#include "Servos.h"
#include "Automode.h"
#include "AlgorithmController.h"
#include "RobotPosition.h"

#define INIT 0
#define STOP 1
#define FOWARD 2
#define LEFT 3
#define RIGHT 4

using namespace cv;
using namespace std;

typedef enum
{
	ROBOT_STATUS_MANUAL = 0,
	ROBOT_STATUS_AUTO,
	ROBOT_STATUS_SUSPEND,
	ROBOT_STATUS_MAX
} T_robot_status;

static UdpSendJpeg    VideoSender;
static UdpSendMap	  *MapSender;
static T_robot_status CurrentStatus;
//static int            EWSNDirection = NORTH;
static bool           HitTheFrontWall;	// TODO: Remove Hit variables..
static bool			  HitTheLeftWall;
static bool 		  HitTheRightWall;
static RobotPosition  CurrentPosition;
static Automode       *AutomodeRobot;
static AlgorithmController *AlgorithmCtrl;
float	      		  ImageOffset;		   // computed robot deviation from the line
int 			      linewidth;

static void  Setup_Control_C_Signal_Handler_And_Keyboard_No_Enter(void);
static void  CleanUp(void);
static void  Control_C_Handler(int s);
static void  HandleInputChar(void);
static void  stopRobot(T_sensor_type sensorType);
static void  avoidLeftWall();
static void  avoidRightWall();
static T_robot_image_info getImageOffset();
void creat_image_capture_thread(void);

//extern static void CallBackRobotTurned();
//extern static void CallBackRobotMoved();

//----------------------------------------------------------------
// main - This is the main program for the line follower and 
// contains the control loop
//-----------------------------------------------------------------
int main(int argc, const char** argv)
{
  if (argc !=4)
  {
      fprintf(stderr,"usage %s hostname video_port map_port\n", argv[0]);
      exit(0);
  }

  Setup_Control_C_Signal_Handler_And_Keyboard_No_Enter(); // Set Control-c handler to properly exit cleanly 

  if (VideoSender.OpenUdp(argv[1],argv[2]) == 0) // Setup remote network destination to send images
  {
	  printf("VideoSender.OpenUdp Failed\n");
	  CleanUp();
	  return(-1);
  }

  AutomodeRobot = new Automode(&CurrentPosition);

  AlgorithmCtrl = new AlgorithmController(AutomodeRobot->getEWSNDirectionFP());
  AlgorithmCtrl->Open();

  AutomodeRobot->setAlgorithmCtrl(AlgorithmCtrl);

  MapSender = new UdpSendMap(AlgorithmCtrl->GetMapFP());

  if (MapSender->OpenUdp(argv[1], argv[3]) == 0) // Setup remote network destination to send images
  {
	  printf("MapSender.OpenUdp Failed\n");
	  CleanUp();
	  return(-1);
  }

  sensor_manager_main(&stopRobot);
  robot_operation_init(getImageOffset, AutomodeRobot->getRobotTurnedFP(), AutomodeRobot->getRobotMovedFP());

  creat_image_capture_thread();

  CurrentStatus = ROBOT_STATUS_MANUAL;
  HitTheFrontWall = false;
  HitTheLeftWall = false;
  HitTheRightWall = false;

  do
  {
	  HandleInputChar();          // Handle Keyboard Input

	  if (CurrentStatus == ROBOT_STATUS_AUTO) {
		  AutomodeRobot->doOperation();
	  } else if (CurrentStatus == ROBOT_STATUS_MANUAL) {
//		  CurrentPosition.SetDirectionToMove(CurrentMovingDirection);
		  // TODO: Next Cell recognition should be implemented.
//		  CurrentPosition.SuccessToMove();
	  } else {
		  printf("Robot is suspend mode.\n");
	  }

	  usleep(2000);			  // sleep 2 milliseconds
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

 MapSender->CloseUdp();
 free(MapSender);

 AlgorithmCtrl->Open();
 free(AlgorithmCtrl);

 free(AutomodeRobot);

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
  if ((ch=getchar())==EOF)              // no key pressed return
     {
       return;
     }

  switch (ch) {
  case 'w':
	  robot_operation_manual(ROBOT_OPERATION_DIRECTION_FORWARD);
	  break;
  case 'x':
	  robot_operation_manual(ROBOT_OPERATION_DIRECTION_BACKWARD);
	  break;
  case 'd':
	  robot_operation_manual(ROBOT_OPERATION_DIRECTION_RIGHT);
	  break;
  case 'a':
	  robot_operation_manual(ROBOT_OPERATION_DIRECTION_LEFT);
	  break;
  case 's':
	  robot_operation_manual(ROBOT_OPERATION_DIRECTION_STOP);
	  CurrentStatus = ROBOT_STATUS_MANUAL;
	  break;
  case 'r':
	  CurrentStatus = ROBOT_STATUS_AUTO;
	  break;
  default:
	  printf("Invalid key input: %c", ch);
	  break;
  }
}

static void stopRobot(T_sensor_type sensorType)
{
	printf("stopRobot(%d) is called!!\n", sensorType);

	switch (sensorType) {
	case SENSOR_TYPE_SONAR:
		HitTheFrontWall = true;

		if (CurrentStatus == ROBOT_STATUS_AUTO) {
			AutomodeRobot->stopRobot();
		} else {
			robot_operation_manual(ROBOT_OPERATION_DIRECTION_STOP);
		}

		HitTheFrontWall = false;
		break;

	case SENSOR_TYPE_LASER_LEFT:
		HitTheLeftWall = true;
		avoidLeftWall();
		HitTheLeftWall = false;
		break;

	case SENSOR_TYPE_LASER_RIGHT:
		HitTheRightWall = true;
		avoidRightWall();
		HitTheRightWall = false;
		break;

	default:
		printf("sensorType(%d) is invalid!", sensorType);
		break;
	}
}

static void avoidLeftWall() {
	printf("avoidLeftWall() is called!\n");

	if (CurrentStatus == ROBOT_STATUS_AUTO) {
		AutomodeRobot->avoidLeftWall();
	} else if (CurrentStatus == ROBOT_STATUS_MANUAL) {
		// TODO: something...
	}
}

static void avoidRightWall() {
	printf("avoidRightWall() is called!\n");

	if (CurrentStatus == ROBOT_STATUS_AUTO) {
		AutomodeRobot->avoidRightWall();
	} else if (CurrentStatus == ROBOT_STATUS_MANUAL) {
		// TODO: something...
	}
}

//----------------------
// Image capture Thread
//----------------------
void *image_capture_thread(void *value) {
	cv::Mat        image;          // camera image in Mat format
	RobotVisionManager rvm;

	rvm.SetDebug(true);	// For debugging

	while (1) {
		rvm.GetCamImage(image);  // Get Camera image

		flip(image, image,-1);       // if running on PI3 flip(-1)=180 degrees

		ImageOffset=rvm.FindLineInImageAndComputeOffsetAndWidth(image, linewidth);

		VideoSender.SetImage(&image);
		usleep(1000);			  // sleep 1 milliseconds
	}
}

void creat_image_capture_thread(void)
{
	pthread_t thread;
	int x = 0;
	// Image capture thread
	pthread_create(&thread, NULL, &image_capture_thread, &x);
}

static T_robot_image_info getImageOffset()
{
	T_robot_image_info imageInfo;
	imageInfo.offset = ImageOffset;
	imageInfo.linewidth = linewidth;
	return imageInfo;
}

//-----------------------------------------------------------------
// END HandleInputChar
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------

