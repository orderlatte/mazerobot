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
static bool           Run=false;

static void  Setup_Control_C_Signal_Handler_And_Keyboard_No_Enter(void);
static void  CleanUp(void);
static void  Control_C_Handler(int s);
static void  HandleInputChar(void);

//----------------------
// Thread Test function
//----------------------
void *thread_test(void *value) {
	while (1) {
		printf("Thread is running!\n");
		sleep(1);
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

  if (!IsPi3) namedWindow( "camera", CV_WINDOW_AUTOSIZE ); // If not running on PI3 open local Window

  // Test Thread
  pthread_t thread;
  int x = 0;
  if (pthread_create(&thread, NULL, &thread_test, &x) != 0) {
	  printf("Filed to create the thread\n");
	  return 1;
  }
 
 //if (!IsPi3) namedWindow( "processed", CV_WINDOW_AUTOSIZE );  // If not running on PI3 open local Window
  do
   {
    iplCameraImage = cvQueryFrame(capture); // Get Camera image
    image= cv::cvarrToMat(iplCameraImage);  // Convert Camera image to Mat format
    if (IsPi3) flip(image, image,-1);       // if running on PI3 flip(-1)=180 degrees

    offset=FindLineInImageAndComputeOffset(image); // Process camera image / locat line and compute offset from line

    SetError(PID,offset); // Set PID with the line offset
    if (Run)              // If in line mode 
      {
       double correction, left, right;
       correction=RunPID(PID);         // compute PID correction
       left = BASESPEED - correction;  // Compute left wheel speed 
       right = BASESPEED + correction; // Compute right wheel speed 
       SetWheelSpeed(left,right);      // Set wheel speeds
       //printf("PID:correction %lf left %lf right %lf delta %lf\n",correction,left,right, left-right);
       //printf("time %ld\n",time_ms());
       //printf("val = %f\n",offset);
      }

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

  printf("pan=%d tilt=%d speed=%d\n",Pan,Tilt,speed);
}
//-----------------------------------------------------------------
// END HandleInputChar
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------

