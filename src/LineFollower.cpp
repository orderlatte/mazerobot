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
#include "PID.h"
#include "PI3OpencvCompat.h"
#include "NetworkUDP.h"
#include "UdpSendRecvJpeg.h"
#include "KeyboardSetup.h"
#include "Servos.h"
#include "ImageProcessing.h"
#include <iostream>

#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.
#include <wiringPi.h>
#endif //UBUNTU

#include "Sonar.h"
#include "user_api/vl53l0x.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#define ObjectNum_0 0
#define ObjectNum_1 1
#define VL53L0X_GOOD_ACCURACY_MODE       0  // Good Accuracy mode
#define VL53L0X_BETTER_ACCURACY_MODE     1  // Better Accuracy mode
#define VL53L0X_BEST_ACCURACY_MODE       2  // Best Accuracy mode
#define VL53L0X_LONG_RANGE_MODE          3  // Longe Range mode
#define VL53L0X_HIGH_SPEED_MODE          4  // High Speed mode#

#define INIT 0
#define STOP 1
#define FOWARD 2
#define LEFT 3
#define RIGHT 4




static uint32_t GetTiming(int object_number); 


using namespace cv;
using namespace std;

// Parameters for PID
#define KP        1.0               // defaut 1.0
#define KI        0.1               // defaut 0.1
#define KD        0.0               // defaut 0.0
#define BASESPEED 6.0               // Range 0-50
#define BASESPEEDFUDGEFACTOR 0.80   // default 1.25

#define MIN_WHEEL_SPEED -50
#define MAX_WHEEL_SPEED  50


#define WIDTH 640
#define HEIGHT 480



static int            AWidth;
static int            AHeight;
static int            Pan;
static int            Tilt;
static CvCapture *    capture=NULL;
static TUdpLocalPort *UdpLocalPort=NULL;
static TUdpDest      *UdpDest=NULL;
static TPID           PID;  
static int           Run=INIT;

int trigger = 28;
int echo = 29;
uint32_t	  timing;





static void  Setup_Control_C_Signal_Handler_And_Keyboard_No_Enter(void);
static void  CleanUp(void);
static void  Control_C_Handler(int s);
static void  HandleInputChar(void);
double sonar(void);
int32_t laser_right(void);
int32_t laser_left(void);


double sonar(void)
{
	double sonar_distance;
	sonar_distance = SonarDistance(30000);
	cout << "Distance is " << sonar_distance << " cm." << endl;
	return sonar_distance;
}

int32_t laser_right(void)
{
#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.

	int32_t distance;
	unsigned int	count=0;
	// Get distance from VL53L0X  on TCA9548A bus 1 
	distance=getDistance(ObjectNum_0);
	if (distance > 0)
        printf("0: %d mm, %d cm, %d\n",distance, (distance/10),count);
	
	usleep(timing);
	count++;
	return distance;
}

int32_t laser_left(void)
{
	int32_t distance;
	unsigned int	count=0;
	// Get distance from VL53L0X  on TCA9548A bus 1 
	distance=getDistance(ObjectNum_1);
	if (distance > 0)
        printf("1: %d mm, %d cm, %d\n",distance, (distance/10),count);
	
	usleep(timing);
	count++;
	return distance;

#else //UBUNTU
	return 0;
#endif //UBUNTU
}


//----------------------
// Sonar Thread
//----------------------
void *sonar_thread(void *value) {
	int32_t		distance;

	while (1) {
		distance = sonar();
		if(distance>0 && distance<5 && Run == FOWARD)
		{
			Run=STOP;                         // Set Run Mode off
			SetWheelSpeed(0,0);        // Stop wheel servos
		}
	}
}

//----------------------
// Sonar Thread
//----------------------
void *laser_thread(void *value) {

	int32_t		distance_left;
	int32_t		distance_right;

	while (1) {
		distance_left = laser_left();
		distance_right = laser_right();
		if(Run == STOP)
		{
			if(distance_left > distance_right)
			{
				Run = LEFT;
			}
			else
			{
				Run = RIGHT;
			}
		}
	}
}

//----------------------------------------------------------------
// main - This is the main program for the line follower and 
// contains the control loop
//-----------------------------------------------------------------
int main(int argc, const char** argv)
{
  IplImage * iplCameraImage; // camera image in IplImage format 
  Mat        image;          // camera image in Mat format 
  float      offset;         // computed robot deviation from the line 



#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.

  if(VL53L0X_i2c_init("/dev/i2c-1")==-1)
  {
   printf("VL53L0X_i2c_init failed\n");
   exit(0);
  }
  
  // Start ranging on TCA9548A bus 1 
  startRanging(ObjectNum_0, VL53L0X_BETTER_ACCURACY_MODE, 0x29, 1, 0x70);
  // Start ranging on TCA9548A bus 2
  startRanging(ObjectNum_1, VL53L0X_BETTER_ACCURACY_MODE, 0x29, 2, 0x70);

#endif //UBUNTU

  
  if ((timing=GetTiming(ObjectNum_0))==0)
  {
   printf("Error Getting Timing Budget\n");
   CleanUp();
  }
  
  if (timing < 20000) timing = 20000;
  printf("Timing %d ms\n", timing/1000);



   if (argc !=3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

  Setup_Control_C_Signal_Handler_And_Keyboard_No_Enter(); // Set Control-c handler to properly exit cleanly 

  if (OpenServos()==-1) // Open the servos so they can be controlled
  {
    printf("Open Servos Failed\n");
    //CleanUp();   // Commented out run on platforms without ServoBlaster
    //return(-1);  // Commented out run on platforms without ServoBlaster
  }
  
  ResetServos(); // Set the servos to Center/Stopped
  sleep(1);      // Wait for the servos to reach position
  SetCameraServosLineTrackMode(Pan,Tilt); // Set Camera to line following position

  InitPID(PID,KP,KI,KD,BASESPEED,BASESPEEDFUDGEFACTOR) ; //initialize the PID

  if  ((UdpLocalPort=OpenUdpPort(0))==NULL)  // Open UDP Network port
     {
       printf("OpenUdpPort Failed\n");
       CleanUp();
       return(-1); 
     }

 if  ((UdpDest=GetUdpDest(argv[1],argv[2]))==NULL)  // Setup remote network destination to send images
     {
       printf("GetUdpDest Failed\n");
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

#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.
  if (wiringPiSetup() == -1) return -1;
#endif //UBUNTU

  SonarInit(trigger, echo);

  if (!IsPi3) namedWindow( "camera", CV_WINDOW_AUTOSIZE ); // If not running on PI3 open local Window
 
 //if (!IsPi3) namedWindow( "processed", CV_WINDOW_AUTOSIZE );  // If not running on PI3 open local Window

  // Sonar thread
  pthread_t thread;
  int x = 0;
  if (pthread_create(&thread, NULL, &sonar_thread, &x) != 0) {
	  printf("Filed to create the thread\n");
	  return 1;
  }

  // Lasor thread
  if (pthread_create(&thread, NULL, &laser_thread, &x) != 0) {
  	  printf("Filed to create the thread\n");
  	  return 1;
   }

  do
   {

    iplCameraImage = cvQueryFrame(capture); // Get Camera image
    image= cv::cvarrToMat(iplCameraImage);  // Convert Camera image to Mat format
    if (IsPi3) flip(image, image,-1);       // if running on PI3 flip(-1)=180 degrees

    offset=FindLineInImageAndComputeOffset(image); // Process camera image / locat line and compute offset from line

    SetError(PID,offset); // Set PID with the line offset


    if (Run == FOWARD)              // If in line mode 
      {
       double correction, left, right;
       correction=RunPID(PID);         // compute PID correction
       left = BASESPEED - correction;  // Compute left wheel speed 
       right = BASESPEED + correction; // Compute right wheel speed 
		SetWheelSpeed(left,right);      // Set wheel speeds
       //printf("PID:correction %lf left %lf right %lf delta %lf\n",correction,left,right, left-right);
       //printf("time %ld\n",time_ms());
       
      }
	else if(Run == LEFT)
	{
		static unsigned char flag = 0;
		double correction, left, right;
       correction=RunPID(PID);         // compute PID correction
       left = -6;  // Compute left wheel speed 
       right = 6; // Compute right wheel speed 
		SetWheelSpeed(left,right);      // Set wheel speeds
		if(flag == 0)
		{
			usleep(900*1000);
		}
		//if(correction < 5 && correction > -5)
		{
			Run = FOWARD;
			flag = 0;
		}
	}
	else if(Run == RIGHT)
	{
		static unsigned char flag = 0;
		double correction, left, right;
       correction=RunPID(PID);         // compute PID correction
       left = 6;  // Compute left wheel speed 
       right = -6; // Compute right wheel speed 
		SetWheelSpeed(left,right);      // Set wheel speeds
		if(flag == 0)
		{
			usleep(900*1000);
		}
		//if(correction < 5 && correction > -5)
		{
			Run = FOWARD;
			flag = 0;
		}
	}
	printf("Run = %d\n",Run);
	printf("val = %f\n",offset);

    UdpSendImageAsJpeg(UdpLocalPort,UdpDest,image);   // Send processed UDP image to detination
  
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

 CloseUdpPort(&UdpLocalPort);      // Close network port
 DeleteUdpDest(&UdpDest);          // Delete Remote Dest
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
  /*
  	if ((ch=='j') || (ch=='l'))           // servo pan keys
     {
      if  (ch=='l')Pan--;               // pan left
      else if  (ch=='j')   Pan++;       // pan right
      SetServoPosition(CAMERA_PAN,Pan); // set pan position
     }
    else if ((ch=='i') || (ch=='m'))     // servo tilt keys
     {
      if (ch=='i')  Tilt--;              // tilt up
      else if  (ch=='m')  Tilt++;        // tilt down
      SetServoPosition(CAMERA_TILT,Tilt);// set tilt position
     }
    else if  (ch=='k')
    {
     Pan=SERVO_CENTER_OR_STOP;           // Center pan and tilt servos
     Tilt=SERVO_CENTER_OR_STOP;
     SetServoPosition(CAMERA_PAN,Pan);
     SetServoPosition(CAMERA_TILT,Tilt);
    }
    else if  (ch=='q')                  // decrease speed
    {
     speed--;
     if (speed<MIN_WHEEL_SPEED) speed=MIN_WHEEL_SPEED;
     SetWheelSpeed(speed,speed);        // set wheel speed
    }
    else if  (ch=='e')                  // increase speed
    {
     speed++;                           
     if (speed>MAX_WHEEL_SPEED) speed=MAX_WHEEL_SPEED;
     SetWheelSpeed(speed,speed);        // set wheel speed
    }
    else if  (ch=='w')
    {
     speed=0;
     SetWheelSpeed(speed,speed);       // Stop wheel servos 
    }
    else if  (ch=='r')
    {
     Run=true;                          // Set Run Mode on
    }
    else if  (ch=='s')
    {
     Run=false;                         // Set Run Mode off
     speed=0;
     SetWheelSpeed(speed,speed);        // Stop wheel servos
    }
    */
	if  (ch=='r')
	{
		Run=FOWARD;                          // Set Run Mode on
    }
	else if  (ch=='s')
    {
     Run=INIT;                         // Set Run Mode off
     speed=0;
     SetWheelSpeed(speed,speed);        // Stop wheel servos
    }
	/*
	else if  (ch=='s')
	{
     	speed=3;
     	SetWheelSpeed(-speed,-speed);       // Stop wheel servos 
	}
	else if(ch=='w')
	{
		speed = 3;
		SetWheelSpeed(speed, speed);
	}
	else if(ch=='d')
	{
		speed = 3;
		SetWheelSpeed(speed, -speed);
	}
	else if(ch=='a')
	{
		speed = 3;
		SetWheelSpeed(-speed, speed);
	}
	else
	{
		speed=0;
    	SetWheelSpeed(speed,speed);       // Stop wheel servos 
	}
	*/
		
	

  printf("pan=%d tilt=%d speed=%d\n",Pan,Tilt,speed);
}
//----------------------------------------------------------------
// VL53L0X_GetTiming
//----------------------------------------------------------------
uint32_t GetTiming(int object_number) 
{
#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.

	VL53L0X_Error status;
	uint32_t      budget;
	VL53L0X_DEV   dev;

	dev=getDev(object_number);
	status=VL53L0X_GetMeasurementTimingBudgetMicroSeconds(dev,&budget);
	if  (status==VL53L0X_ERROR_NONE)
	{
		printf("sleep time %d\n",budget);
		return(budget+1000);
	}
	else return (0);
 
#else //UBUNTU
	return 0;
#endif //UBUNTU
}



//-----------------------------------------------------------------
// END HandleInputChar
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------

