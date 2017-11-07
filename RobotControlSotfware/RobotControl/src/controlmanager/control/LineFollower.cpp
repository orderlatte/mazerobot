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
#include "UdpSendJpeg.h"
#include "Direction.h"
#include "RobotPosition.h"
#include "sensor_manager.h"
#include "servos_manager.h"
#include "Servos.h"

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

typedef struct
{
	bool CheckedFrontWall = true;
	bool CheckedLeftWall = true;
	bool CheckedRightWall = true;
	bool CheckedBackWall = true;
	bool ConfirmedFrontWall = false;
	bool ConfirmedLeftWall = false;
	bool ConfirmedRightWall = false;
	bool ConfirmedBackWall = true;
}T_WallData;


static UdpSendJpeg    VideoSender;
static UdpSendMap	  MapSender;
static float	      ImageOffset;		   // computed robot deviation from the line
static T_robot_status CurrentStatus;
static T_robot_moving_direction CurrentMovingDirection;
static int            EWSNDirection = NORTH;
static bool           HitTheFrontWall;
static bool			  HitTheLeftWall;
static bool 		  HitTheRightWall;
static RobotPosition  CurrentPosition;

static void  Setup_Control_C_Signal_Handler_And_Keyboard_No_Enter(void);
static void  CleanUp(void);
static void  Control_C_Handler(int s);
static void  HandleInputChar(void);
static bool  handleWhenFrontWallIsExisted();
static void  moveNextCellAuto();
static void  stopRobot(T_sensor_type sensorType);
static void  avoidLeftWall();
static void  avoidRightWall();
static float getImageOffset();
static int getNextEWSNDirection(bool initialStarted);
void creat_image_capture_thread(void);

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

  if (MapSender.OpenUdp(argv[1], argv[3]) == 0) // Setup remote network destination to send images
  {
	  printf("MapSender.OpenUdp Failed\n");
	  CleanUp();
	  return(-1);
  }

  sensor_manager_main(&stopRobot);
  servos_manager_main();

  creat_image_capture_thread();

  CurrentStatus = ROBOT_STATUS_MANUAL;
  CurrentMovingDirection = ROBOT_MOVING_DIRECTION_STOP;
  HitTheFrontWall = false;
  HitTheLeftWall = false;
  HitTheRightWall = false;

  do
  {
	  HandleInputChar();          // Handle Keyboard Input

	  if (CurrentStatus == ROBOT_STATUS_AUTO) {
		  // TODO: It should be call back function.
		  EWSNDirection = getNextEWSNDirection(false);
		  CurrentMovingDirection = CurrentPosition.SetEWSNDirectionToMove(EWSNDirection);
		  moveNextCellAuto();	// For testing..
		  CurrentPosition.SuccessToMove();
	  } else if (CurrentStatus == ROBOT_STATUS_MANUAL) {
		  CurrentPosition.SetDirectionToMove(CurrentMovingDirection);
		  // TODO: Next Cell recognition should be implemented.
		  CurrentPosition.SuccessToMove();
	  } else {
		  printf("Robot is suspend mode.\n");
		  CurrentMovingDirection = ROBOT_MOVING_DIRECTION_STOP;
		  moveNextCellAuto();
	  }

	  usleep(2000);			  // sleep 2 milliseconds
  } while (1);


  return 0;
}

static void sendCurrentRobotStatus(T_SensorData* sensorData, T_WallData *wallData, bool initialStarted) {

	int EWSN = NORTH;

	if (sensorData->sonar_distance < 10) { // Less than 10cm
		wallData->ConfirmedFrontWall = true;
	}

	if (sensorData->laser_left_distance < 10) {  // Less than
		wallData->ConfirmedLeftWall = true;
	}

	if (sensorData->laser_right_distance < 10) {  // Less than
		wallData->ConfirmedRightWall = true;
	}

	if (initialStarted == true) {
		wallData->CheckedBackWall = false;
		wallData->ConfirmedBackWall = false;
	}

	EWSN = CurrentPosition.getCurrentEWSNDirection();

	// TODO: Get sign position
	// TODO: Get sign type
	// TODO: Get red dot

	// TODO: Send to Maze Algorithm component
}

// It's for testing..
static int getTestNextDirection(T_WallData *wallData) {
	int EWSN = CurrentPosition.getCurrentEWSNDirection();

	switch (EWSN) {
	case EAST:
		if (wallData->CheckedFrontWall == false) {
			return EAST;
		}

		if (wallData->CheckedLeftWall == false) {
			return NORTH;
		}

		if (wallData->CheckedRightWall == false) {
			return SOUTH;
		}

		return WEST;

	case WEST:
		if (wallData->CheckedFrontWall == false) {
			return WEST;
		}

		if (wallData->CheckedLeftWall == false) {
			return SOUTH;
		}

		if (wallData->CheckedRightWall == false) {
			return NORTH;
		}

		return EAST;

	case SOUTH:
		if (wallData->CheckedFrontWall == false) {
			return SOUTH;
		}

		if (wallData->CheckedLeftWall == false) {
			return EAST;
		}

		if (wallData->CheckedRightWall == false) {
			return WEST;
		}

		return NORTH;

	case NORTH:
		if (wallData->CheckedFrontWall == false) {
			return NORTH;
		}

		if (wallData->CheckedLeftWall == false) {
			return WEST;
		}

		if (wallData->CheckedRightWall == false) {
			return EAST;
		}

		return SOUTH;

	default:
		printf("It is not supported EWSN direction(%d)!", EWSN);
		return NORTH;
	}
}

static int getNextEWSNDirection(bool initialStarted) {
	T_SensorData sensorData = get_sensor_data();
	T_WallData wallData;

	sendCurrentRobotStatus(&sensorData, &wallData, initialStarted);

	// TODO: Below code will be replaced with Maze algorithm
	return getTestNextDirection(&wallData);
}





static void moveNextCellAuto() {
	printf("handleWhenFrontWallIsExisted() is called!\n");

	switch (CurrentMovingDirection) {
	case ROBOT_MOVING_DIRECTION_FORWARD:
		robot_mode_setting(ROBOT_FORWARD_MOVING,ImageOffset);	// Backward
		usleep(3000000);	// 3sec
		break;

	case ROBOT_MOVING_DIRECTION_BACK:
		robot_mode_setting(ROBOT_BACKWARD_MOVING,ImageOffset);	// Backward
		usleep(3000000);	// 3sec
		break;

	case ROBOT_MOVING_DIRECTION_LEFT:
		robot_mode_setting(ROBOT_LEFT_ROTATING,ImageOffset);	// Left
		usleep(3000000);	// 3sec
		robot_mode_setting(ROBOT_FORWARD_MOVING,ImageOffset);	// Backward
		usleep(3000000);	// 3sec
		break;

	case ROBOT_MOVING_DIRECTION_RIGHT:
		robot_mode_setting(ROBOT_RIGHT_ROTATING,ImageOffset);	// Right
		usleep(3000000);	// 3sec
		robot_mode_setting(ROBOT_FORWARD_MOVING,ImageOffset);	// Backward
		usleep(3000000);	// 3sec
		break;

	case ROBOT_MOVING_DIRECTION_STOP:
		robot_mode_setting(ROBOT_STOP,ImageOffset);	// Right
		break;

	default:
		printf("Robot could not handle in this case!\n");
		break;
	}
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

 VideoSender.CloseUdp();
 MapSender.CloseUdp();
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
	  robot_mode_setting(ROBOT_FORWARD_MOVING,ImageOffset);
	  break;
  case 'x':
	  robot_mode_setting(ROBOT_BACKWARD_MOVING,ImageOffset);
	  break;
  case 'd':
	  robot_mode_setting(ROBOT_RIGHT_ROTATING,ImageOffset);
	  break;
  case 'a':
	  robot_mode_setting(ROBOT_LEFT_ROTATING,ImageOffset);
	  break;
  case 's':
	  robot_mode_setting(ROBOT_STOP,ImageOffset);
	  CurrentStatus = ROBOT_STATUS_MANUAL;
	  break;
  case 'r':
	  CurrentStatus = ROBOT_STATUS_AUTO;
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
		robot_mode_setting(ROBOT_STOP,ImageOffset);	// Stop
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

	switch (CurrentMovingDirection) {
	case ROBOT_MOVING_DIRECTION_FORWARD:
		robot_mode_setting(ROBOT_STOP,ImageOffset);	// Stop
		robot_mode_setting(ROBOT_RIGHT_ROTATING,ImageOffset);	// Right
		usleep(500000);	// sleep 500 milliseconds
		robot_mode_setting(ROBOT_STOP,ImageOffset);	// Stop
		break;

	case ROBOT_MOVING_DIRECTION_BACK:
		robot_mode_setting(ROBOT_STOP,ImageOffset);	// Stop
		robot_mode_setting(ROBOT_LEFT_ROTATING,ImageOffset);	// Left
		usleep(500000);	// sleep 500 milliseconds
		robot_mode_setting(ROBOT_STOP,ImageOffset);	// Stop
		break;

	case ROBOT_MOVING_DIRECTION_LEFT:
		robot_mode_setting(ROBOT_STOP,ImageOffset);	// Stop
		robot_mode_setting(ROBOT_RIGHT_ROTATING,ImageOffset);	// Right
		usleep(500000);	// sleep 500 milliseconds
		robot_mode_setting(ROBOT_STOP,ImageOffset);	// Stop
		break;

	default:
		printf("Robot could not handle in this case!\n");
		break;
	}
}

static void avoidRightWall() {
	printf("avoidRightWall() is called!\n");

	switch (CurrentMovingDirection) {
	case ROBOT_MOVING_DIRECTION_FORWARD:
		robot_mode_setting(ROBOT_LEFT_ROTATING,ImageOffset);	// Left
		usleep(500000);	// sleep 500 milliseconds
		robot_mode_setting(ROBOT_STOP,ImageOffset);	// Right
		break;

	case ROBOT_MOVING_DIRECTION_BACK:
		robot_mode_setting(ROBOT_RIGHT_ROTATING,ImageOffset);	// Right
		usleep(500000);	// sleep 500 milliseconds
		robot_mode_setting(ROBOT_STOP,ImageOffset);	// Left
		break;

	case ROBOT_MOVING_DIRECTION_RIGHT:
		robot_mode_setting(ROBOT_LEFT_ROTATING,ImageOffset);	// Left
		usleep(500000);	// sleep 500 milliseconds
		robot_mode_setting(ROBOT_STOP,ImageOffset);	// Left
		break;

	default:
		printf("Robot could not handle in this case!\n");
		break;
	}
}

//----------------------
// Image capture Thread
//----------------------
void *image_capture_thread(void *value) {
	Mat        image;          // camera image in Mat format
	RobotVisionManager rvm;

//	rvm.SetDebug(true);	// For debugging

	while (1) {
		rvm.GetCamImage(image);  // Get Camera image

		flip(image, image,-1);       // if running on PI3 flip(-1)=180 degrees

		ImageOffset=rvm.FindLineInImageAndComputeOffset(image);

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

static float getImageOffset()
{
	return ImageOffset;
}

//-----------------------------------------------------------------
// END HandleInputChar
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------

