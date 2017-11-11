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
#include "Manualmode.h"
#include "AlgorithmController.h"
#include "RobotPosition.h"
#include "FloorFinder.h"
#include "StartingPoint.h"

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
static UdpSendMap	  *MapSender = NULL;
static T_robot_status PreviousStatus;
static T_robot_status CurrentStatus;
//static int            EWSNDirection = NORTH;
static bool           HitTheFrontWall;	// TODO: Remove Hit variables..
static bool			  HitTheLeftWall;
static bool 		  HitTheRightWall;
static RobotPosition  CurrentPosition;
static Automode       *AutomodeRobot = NULL;
static Manualmode     *ManualmodeRobot = NULL;
static AlgorithmController *AlgorithmCtrl = NULL;
static bool 		  toggle_mode_debug = false;
static bool			  isResumeAutomode = false;
static FloorFinder	  *FloorData = NULL;
static StartingPoint  *StartingData = NULL;
float	      		  ImageOffset;		   // computed robot deviation from the line
int 			      linewidth;

// TODO: Below codes should move to UI interaction code
typedef enum
{
	UI_COMMAND_MODE_MANUAL = 0,
	UI_COMMAND_MODE_AUTO,
	UI_COMMAND_DIRECTION_FORWARD,
	UI_COMMAND_DIRECTION_BACK,
	UI_COMMAND_DIRECTION_TURN_LEFT,
	UI_COMMAND_DIRECTION_TURN_RIGHT,
	UI_COMMAND_DIRECTION_STOP,
	UI_COMMAND_CELL_MOVED,
	UI_COMMAND_CAMERA_LEFT,
	UI_COMMAND_CAMERA_RIGHT,
	UI_COMMAND_CAMERA_UPPER,
	UI_COMMAND_CAMERA_LOWER,
	UI_COMMAND_CAMERA_CENTER,
	UI_COMMAND_CAMERA_LEFT_WALL,
	UI_COMMAND_CAMERA_RIGHT_WALL,
	UI_COMMAND_MAX
} T_ui_command;
typedef void (*fp_ui_command)(T_ui_command command);
static fp_ui_command fpUiCommand;
void ui_command_init(fp_ui_command fp);
/////////////////

static void  Setup_Control_C_Signal_Handler_And_Keyboard_No_Enter(void);
static void  CleanUp(void);
static void  Control_C_Handler(int s);
static void  HandleInputChar();
static void  stopRobot(T_sensor_type sensorType);
static void  avoidLeftWall();
static void  avoidRightWall();
static T_robot_image_info getImageOffset();
void creat_image_capture_thread(FloorFinder *floorData);
void CallBackHandleUiCommand (T_ui_command command);
void CallBackAutomodeFail();
void recognizeFloor(RobotVisionManager *rvm, FloorFinder *floorData) ;

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

  FloorData = new FloorFinder();
  StartingData = new StartingPoint(CurrentPosition.GetX(), CurrentPosition.GetY());

  AlgorithmCtrl = new AlgorithmController(StartingData);
  if (AlgorithmCtrl->Open() == false) {
	  printf("main() - Open() is failed!\n");
  }


  AutomodeRobot = new Automode(&CurrentPosition, CallBackAutomodeFail, AlgorithmCtrl, FloorData);
  ManualmodeRobot = new Manualmode(&CurrentPosition, AlgorithmCtrl);

//  AutomodeRobot->setAlgorithmCtrl(AlgorithmCtrl);
//  ManualmodeRobot->setAlgorithmCtrl(AlgorithmCtrl);

  MapSender = new UdpSendMap(AlgorithmCtrl->GetMapFP());

  if (MapSender->OpenUdp(argv[1], argv[3]) == 0) // Setup remote network destination to send images
  {
	  printf("MapSender.OpenUdp Failed\n");
	  CleanUp();
	  return(-1);
  }

  sensor_manager_main(&stopRobot);
  robot_operation_init(getImageOffset, AutomodeRobot->getRobotTurnedFP(), AutomodeRobot->getRobotMovedFP());

  creat_image_capture_thread(FloorData);

  ui_command_init(CallBackHandleUiCommand);

  PreviousStatus = ROBOT_STATUS_MANUAL;
  CurrentStatus = ROBOT_STATUS_MANUAL;
  HitTheFrontWall = false;
  HitTheLeftWall = false;
  HitTheRightWall = false;



  // TODO: Reset..! Add feature!

  do
  {
	  if (CurrentStatus == ROBOT_STATUS_AUTO) {
		  // TODO: Restore status from manual mode
		  if (PreviousStatus == ROBOT_STATUS_MANUAL) {
			  if (isResumeAutomode == false) {
				  AutomodeRobot->init();
				  isResumeAutomode = true;
			  } else {
				  AutomodeRobot->resume();
			  }
			  PreviousStatus = CurrentStatus;
		  }

		  AutomodeRobot->doOperation();
	  } else if (CurrentStatus == ROBOT_STATUS_MANUAL) {
		  if (PreviousStatus == ROBOT_STATUS_AUTO) {
			  ManualmodeRobot->init();
			  PreviousStatus = CurrentStatus;
		  }

		  ManualmodeRobot->doOperation();
	  } else {
		  printf("Robot is suspend mode.\n");
	  }

	  usleep(2000);			  // sleep 2 milliseconds
  } while (1);


  return 0;
}

void CallBackAutomodeFail() {
	CurrentStatus = ROBOT_STATUS_MANUAL;
	toggle_mode_debug = false;
}

void CallBackHandleUiCommand (T_ui_command command) {

	switch (command) {
		case UI_COMMAND_MODE_MANUAL:
			CurrentStatus = ROBOT_STATUS_MANUAL;
			ManualmodeRobot->setCommand(MANUALMODE_CMD_READY);
			break;

		case UI_COMMAND_MODE_AUTO:
			CurrentStatus = ROBOT_STATUS_AUTO;
			break;

		case UI_COMMAND_DIRECTION_FORWARD:
			if (CurrentStatus == ROBOT_STATUS_AUTO) {
				printf("CallBackHandleUiCommand() - Current mode is automode!\n");
				return;
			}
			ManualmodeRobot->setCommand(MANUALMODE_CMD_FORWARD);
			break;

		case UI_COMMAND_DIRECTION_BACK:
			if (CurrentStatus == ROBOT_STATUS_AUTO) {
				printf("CallBackHandleUiCommand() - Current mode is automode!\n");
				return;
			}
			ManualmodeRobot->setCommand(MANUALMODE_CMD_BACK);
			break;

		case UI_COMMAND_DIRECTION_TURN_LEFT:
			if (CurrentStatus == ROBOT_STATUS_AUTO) {
				printf("CallBackHandleUiCommand() - Current mode is automode!\n");
				return;
			}
			ManualmodeRobot->setCommand(MANUALMODE_CMD_TURN_LEFT);
			break;

		case UI_COMMAND_DIRECTION_TURN_RIGHT:
			if (CurrentStatus == ROBOT_STATUS_AUTO) {
				printf("CallBackHandleUiCommand() - Current mode is automode!\n");
				return;
			}
			ManualmodeRobot->setCommand(MANUALMODE_CMD_TURN_RIGHT);
			break;

		case UI_COMMAND_DIRECTION_STOP:
			if (CurrentStatus == ROBOT_STATUS_AUTO) {
				printf("CallBackHandleUiCommand() - Current mode is automode!\n");
				return;
			}
			ManualmodeRobot->setCommand(MANUALMODE_CMD_STOP);
			break;

		case UI_COMMAND_CELL_MOVED:
			if (CurrentStatus == ROBOT_STATUS_AUTO) {
				printf("CallBackHandleUiCommand() - Current mode is automode!\n");
				return;
			}
			ManualmodeRobot->setCommand(MANUALMODE_CMD_CELL_MOVED);
			break;

		case UI_COMMAND_CAMERA_LEFT:
			if (CurrentStatus == ROBOT_STATUS_AUTO) {
				printf("CallBackHandleUiCommand() - Current mode is automode!\n");
				return;
			}
			ManualmodeRobot->setCommand(MANUALMODE_CMD_CAMERA_LEFT);
			break;

		case UI_COMMAND_CAMERA_RIGHT:
			if (CurrentStatus == ROBOT_STATUS_AUTO) {
				printf("CallBackHandleUiCommand() - Current mode is automode!\n");
				return;
			}
			ManualmodeRobot->setCommand(MANUALMODE_CMD_CAMERA_RIGHT);
			break;

		case UI_COMMAND_CAMERA_UPPER:
			if (CurrentStatus == ROBOT_STATUS_AUTO) {
				printf("CallBackHandleUiCommand() - Current mode is automode!\n");
				return;
			}
			ManualmodeRobot->setCommand(MANUALMODE_CMD_CAMERA_UPPER);
			break;

		case UI_COMMAND_CAMERA_LOWER:
			if (CurrentStatus == ROBOT_STATUS_AUTO) {
				printf("CallBackHandleUiCommand() - Current mode is automode!\n");
				return;
			}
			ManualmodeRobot->setCommand(MANUALMODE_CMD_CAMERA_LOWER);
			break;

		case UI_COMMAND_CAMERA_CENTER:
			if (CurrentStatus == ROBOT_STATUS_AUTO) {
				printf("CallBackHandleUiCommand() - Current mode is automode!\n");
				return;
			}
			ManualmodeRobot->setCommand(MANUALMODE_CMD_CAMERA_CENTER);
			break;

		case UI_COMMAND_CAMERA_LEFT_WALL:
			if (CurrentStatus == ROBOT_STATUS_AUTO) {
				printf("CallBackHandleUiCommand() - Current mode is automode!\n");
				return;
			}
			ManualmodeRobot->setCommand(MANUALMODE_CMD_CAMERA_LEFT_WALL);
			break;

		case UI_COMMAND_CAMERA_RIGHT_WALL:
			if (CurrentStatus == ROBOT_STATUS_AUTO) {
				printf("CallBackHandleUiCommand() - Current mode is automode!\n");
				return;
			}
			ManualmodeRobot->setCommand(MANUALMODE_CMD_CAMERA_RIGHT_WALL);
			break;

		default:
			printf("handleUiCommand() - It's not supported command! (%d)\n", command);
			break;
	}
}

void *ui_command_thread(void *value) {
	do {
		HandleInputChar();          // Handle Keyboard Input
		usleep(1000);			// sleep 1 milliseconds
	} while(1);
}

// TODO: Test code. It should be move to UI network interface
void ui_command_init(fp_ui_command fp)
{
	pthread_t thread1;
	int x = 0;
	fpUiCommand = fp;
	pthread_create(&thread1, NULL, &ui_command_thread, &x);
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

 AlgorithmCtrl->Close();
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
static void HandleInputChar()
{
  int ch;
  if ((ch=getchar())==EOF)              // no key pressed return
     {
       return;
     }

  switch (ch) {
  case 'w':
	  CallBackHandleUiCommand(UI_COMMAND_DIRECTION_FORWARD);
	  break;

  case 'x':
	  CallBackHandleUiCommand(UI_COMMAND_DIRECTION_BACK);
	  break;

  case 'd':
	  CallBackHandleUiCommand(UI_COMMAND_DIRECTION_TURN_RIGHT);
	  break;

  case 'a':
	  CallBackHandleUiCommand(UI_COMMAND_DIRECTION_TURN_LEFT);
	  break;

  case 's':
	  CallBackHandleUiCommand(UI_COMMAND_DIRECTION_STOP);
	  break;

  case 'r':
	  if (toggle_mode_debug == false) {
		  CallBackHandleUiCommand(UI_COMMAND_MODE_AUTO);
		  toggle_mode_debug = true;
	  } else {
		  CallBackHandleUiCommand(UI_COMMAND_MODE_MANUAL);
		  toggle_mode_debug = false;
	  }
	  break;

  case 'v':
	  CallBackHandleUiCommand(UI_COMMAND_CELL_MOVED);
	  break;

  case 'i':
	  CallBackHandleUiCommand(UI_COMMAND_CAMERA_UPPER);
	  break;

  case 'j':
	  CallBackHandleUiCommand(UI_COMMAND_CAMERA_LEFT);
	  break;

  case 'l':
	  CallBackHandleUiCommand(UI_COMMAND_CAMERA_RIGHT);
	  break;

  case 'm':
	  CallBackHandleUiCommand(UI_COMMAND_CAMERA_LOWER);
	  break;

  case 'k':
	  CallBackHandleUiCommand(UI_COMMAND_CAMERA_CENTER);
	  break;

  case 'u':
	  CallBackHandleUiCommand(UI_COMMAND_CAMERA_LEFT_WALL);
	  break;

  case 'o':
	  CallBackHandleUiCommand(UI_COMMAND_CAMERA_RIGHT_WALL);
	  break;

  default:
	  printf("Invalid key input: %c\n", ch);
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
	cv::Mat image;          // camera image in Mat format
	FloorFinder *floor = (FloorFinder *) value;

	RobotVisionManager rvm;

	rvm.SetDebug(true);	// For debugging

	while (1) {
		rvm.GetCamImage(image);  // Get Camera image
		if (IsPi3 == true) flip(image, image,-1);       // if running on PI3 flip(-1)=180 degrees

		ImageOffset=rvm.FindLineInImageAndComputeOffsetAndWidth(image, linewidth);

		if (linewidth > 190) {
			recognizeFloor(&rvm, floor);
		}

		VideoSender.SetImage(&image);
		usleep(1000);			  // sleep 1 milliseconds
	}
}

void recognizeFloor(RobotVisionManager *rvm, FloorFinder *floorData) {
	cv::Mat redDotImage;
	cv::Mat goalImage;

	floorData->init();

	// Get new floor image to find red dot
	rvm->GetCamImage(redDotImage);
	if (IsPi3 == true) flip(redDotImage, redDotImage,-1);       // if running on PI3 flip(-1)=180 degrees
	if (rvm->FindRedDot(redDotImage) == true) {
		floorData->RedDot = true;
	}

	// Get new floor image to find goal
	rvm->GetCamImage(goalImage);
	if (IsPi3 == true) flip(goalImage, goalImage,-1);       // if running on PI3 flip(-1)=180 degrees
	if (rvm->FindGoalArea(goalImage) == true) {
		floorData->Goal = true;
	}
}

void creat_image_capture_thread(FloorFinder *floorData)
{
	pthread_t thread;
//	int x = 0;
	// Image capture thread
	pthread_create(&thread, NULL, &image_capture_thread, floorData);
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

