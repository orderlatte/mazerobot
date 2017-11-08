#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>

#include "PID.h"
#include "Servos.h"
#include "servos_manager.h"

#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.
#include <wiringPi.h>
#endif //UBUNTU


using namespace std;

// Parameters for PID
#define KP        1.0               // defaut 1.0
#define KI        0.01               // defaut 0.1
#define KD        0.0               // defaut 0.0
#define BASESPEED 10.0               // Range 0-50
#define M_BASESPEED 6.0               // Range 0-50

#define BASESPEEDFUDGEFACTOR 0.80   // default 1.25

#define MIN_WHEEL_SPEED -50
#define MAX_WHEEL_SPEED  50


static int            Pan;
static int            Tilt;
static TPID           PID;


void servos_manager_init(void)
{
	static char servos_manager_init_flag = 0;

	if(servos_manager_init_flag == 0)
	{
		if (OpenServos()==-1) // Open the servos so they can be controlled
		{
			printf("Open Servos Failed\n");
		}
  
		ResetServos(); // Set the servos to Center/Stopped
		sleep(1);      // Wait for the servos to reach position
		SetCameraServosLineTrackMode(Pan,Tilt); // Set Camera to line following position
		InitPID(PID,KP,KI,KD,BASESPEED,BASESPEEDFUDGEFACTOR) ; //initialize the PID

		servos_manager_init_flag = 1;
	}
}


void robot_line_tracking_operation(float offset)
{
	double correction, left, right;
	SetError(PID,offset); // Set PID with the line offset
	correction = RunPID(PID);         // compute PID correction
	left = BASESPEED - correction;  // Compute left wheel speed 
	right = BASESPEED + correction; // Compute right wheel speed 
	SetWheelSpeed(left,right);      // Set wheel speeds
}


void robot_mode_setting(T_robot_moving_mode robot_moving_mode, float offset)
{
	switch(robot_moving_mode)
	{
		case ROBOT_STOP:
			SetWheelSpeed(0,0);
		break;

		case ROBOT_LINE_TRACKING:
			robot_line_tracking_operation(offset);
		break;

		case ROBOT_FORWARD_MOVING:
			SetWheelSpeed(M_BASESPEED, M_BASESPEED);
		break;

		case ROBOT_BACKWARD_MOVING:
			SetWheelSpeed(-M_BASESPEED, -M_BASESPEED);
		break;

		case ROBOT_LEFT_ROTATING:
			SetWheelSpeed(-M_BASESPEED, M_BASESPEED);
		break;

		case ROBOT_RIGHT_ROTATING:
			SetWheelSpeed(M_BASESPEED, -M_BASESPEED);
		break;

		default:
			SetWheelSpeed(0,0);
		break;
	}
	
}


void servos_manager_main(void)
{
	servos_manager_init();
}




