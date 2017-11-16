#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <map>
#include <fstream>


#include "PID.h"
#include "Servos.h"
#include "servos_manager.h"

#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.
#include <wiringPi.h>
#endif //UBUNTU


using namespace std;

double pid_kp;
double pid_ki;
double pid_kd;
TPID PID;


map<string, float> PID_Param;


void PID_readParameter(void)
{
	string tmp;
	float tmpFloat;

	ifstream inFile("PIDParameter.txt");

	if (inFile.is_open())
	{
		while (!inFile.eof()) {
			inFile >> tmp;
			inFile >> tmpFloat;
			PID_Param[tmp] = tmpFloat;
		}
		cout << "PIDParameter reading complete!!!" << endl;
	}
	else
	{
		cerr << "Can't open!!! : PIDParameter.txt" << endl;
	}

	inFile.close();

	map<string, float>::iterator itr;

	itr = PID_Param.find("pid_kp");
	if (PID_Param.end() != itr) pid_kp = itr->second;

	itr = PID_Param.find("pid_ki");
	if (PID_Param.end() != itr) pid_ki = itr->second;

    itr = PID_Param.find("pid_kd");
	if (PID_Param.end() != itr) pid_kd = itr->second;

}


void servos_manager_init(void)
{
	static char servos_manager_init_flag = 0;

	if(servos_manager_init_flag == 0)
	{
		if (OpenServos()==-1) // Open the servos so they can be controlled
		{
			printf("Open Servos Failed\n");
		}
		PID_readParameter();
  
		ResetServos(); // Set the servos to Center/Stopped
		sleep(1);      // Wait for the servos to reach position
		InitPID(PID,pid_kp,pid_ki,pid_kd,BASESPEED,BASESPEEDFUDGEFACTOR) ;

		servos_manager_init_flag = 1;
	}
}

void servos_cam_operation(int pan, int tilt)
{
	SetCameraServos(pan,tilt);
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
	static unsigned long start_time;
	static unsigned long execute_time;
	static unsigned char pid_flag;

#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.
	if(pid_flag == 0)
	{
		start_time = micros();
		pid_flag = 1;
	}
	else if(pid_flag == 1)
	{
		execute_time = micros() - start_time;
		if(execute_time > (50*1000))
		{
			pid_flag = 2;
		}
	}
	else
	{
		pid_flag = 0;
	}
#endif

	
	switch(robot_moving_mode)
	{
		case ROBOT_STOP:
			SetWheelSpeed(0,0);
		break;

		case ROBOT_LINE_TRACKING:
			if(pid_flag == 2)
			{
				robot_line_tracking_operation(offset);
			}
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




