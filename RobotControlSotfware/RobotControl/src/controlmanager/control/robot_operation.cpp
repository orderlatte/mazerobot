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
#include "UdpSendMap.h"
#include "KeyboardSetup.h"

#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.
#include <wiringPi.h>
#endif //UBUNTU

#include "sensor_manager.h"
#include "servos_manager.h"
#include "robot_operation.h"

using namespace cv;
using namespace std;

extern int linewidth;
extern float ImageOffset;		   // computed robot deviation from the line
T_robot_operation_info robot_operation_info;

static fp_get_image_offset fpGetImageOffset;
static fp_robot_turned fpRobotTurned;
static fp_robot_moved fpRobotMoved;

long micros_wrapper();


void robot_move_one_cell_foward(void)
{
	static unsigned char robot_move_one_cell_foward_state = 0;
	static unsigned long timeoutstart = 0;

	if(robot_move_one_cell_foward_state == 0)
	{
		timeoutstart = micros_wrapper();
		robot_mode_setting(ROBOT_LINE_TRACKING, ImageOffset);
		robot_move_one_cell_foward_state = 1;
	}
	else if(robot_move_one_cell_foward_state == 1)
	{
		robot_mode_setting(ROBOT_LINE_TRACKING, ImageOffset);
		if(micros_wrapper()-timeoutstart > (1000*1000))
		{
			robot_move_one_cell_foward_state = 2;
		}
	}
	else if(robot_move_one_cell_foward_state == 2)
	{
		robot_mode_setting(ROBOT_LINE_TRACKING, ImageOffset);
		if(linewidth < 190 && linewidth > 130)
		{
			;
		}
		else
		{
			robot_move_one_cell_foward_state = 3;
			timeoutstart = micros_wrapper();
		}
	}
	else if(robot_move_one_cell_foward_state == 3)
	{
		if(micros_wrapper()-timeoutstart < (800*1000))
		{
			robot_mode_setting(ROBOT_LINE_TRACKING, ImageOffset);
		}
		else
		{
			robot_mode_setting(ROBOT_STOP, ImageOffset);
			robot_move_one_cell_foward_state = 0;
			robot_operation_info.robot_run = 0;
		}
	}

}

void robot_trun_move_onecell(T_robot_operation_direction direction)
{
	static unsigned char robot_turn_to_cross_state = 0;
	static unsigned long timeoutstart = 0;

	if(robot_turn_to_cross_state == 0)
	{
		timeoutstart = micros_wrapper();
		robot_turn_to_cross_state = 1;
	}
	else if(robot_turn_to_cross_state == 1)
	{
		if(direction == ROBOT_OPERATION_DIRECTION_LEFT)
		{
			robot_mode_setting(ROBOT_LEFT_ROTATING,ImageOffset);
		}
		else
		{
			robot_mode_setting(ROBOT_RIGHT_ROTATING,ImageOffset);
		}

		if(micros_wrapper()-timeoutstart > (900*1000))
		{
			if((linewidth < 160 && linewidth > 140) || (micros_wrapper()-timeoutstart > (1200*1000)))
			{
				robot_mode_setting(ROBOT_STOP, ImageOffset);
				robot_operation_info.direction = ROBOT_OPERATION_DIRECTION_FORWARD;
				robot_turn_to_cross_state = 0;
			}
		}
	}
}

void robot_back_move_one_cell(void)
{
	
	static unsigned char robot_turn_to_cross_state = 0;
	static unsigned long timeoutstart = 0;
	
	if(robot_turn_to_cross_state == 0)
	{
		timeoutstart = micros_wrapper();
		robot_turn_to_cross_state = 1;
	}
	else if(robot_turn_to_cross_state == 1)
	{
		//robot_mode_setting(ROBOT_LEFT_ROTATING,offset);
		robot_mode_setting(ROBOT_RIGHT_ROTATING,ImageOffset);
		if(micros_wrapper()-timeoutstart > (2200*1000))
		{
			if((linewidth < 160 && linewidth > 140) || (micros_wrapper()-timeoutstart > (1200*1000)))
			{
				robot_mode_setting(ROBOT_STOP, ImageOffset);
				robot_operation_info.direction = ROBOT_OPERATION_DIRECTION_FORWARD;
				robot_turn_to_cross_state = 0;
			}
		}
	}
}

void robot_operation_manual(T_robot_operation_direction direction)
{
	robot_operation_info.direction = direction;
	robot_operation_info.robot_run = 1;
	robot_operation_info.mode = ROBOT_OPERATION_MANUAL;
}

void robot_operation_auto(T_robot_operation_direction direction)
{
	robot_operation_info.direction = direction;
	robot_operation_info.robot_run = 1;
	robot_operation_info.mode = ROBOT_OPERATION_AUTO;
}

void robot_operation_manual_operation(void)
{
;
}

void robot_operation_auto_operation(void)
{
	if(robot_operation_info.robot_run == 1)
	{
		switch(robot_operation_info.direction)
		{
			case ROBOT_OPERATION_DIRECTION_FORWARD:
				robot_move_one_cell_foward();
				break;
			case ROBOT_OPERATION_DIRECTION_LEFT:
			case ROBOT_OPERATION_DIRECTION_RIGHT:
				robot_trun_move_onecell(robot_operation_info.direction);
				break;
			case ROBOT_OPERATION_DIRECTION_BACKWARD:
				robot_back_move_one_cell();
				break;
			default:
				robot_mode_setting(ROBOT_STOP, ImageOffset);
				break;
				
		}
	}
				
}

void *robot_operation_main(void *value)
{
	while(1)
	{
		usleep(30*1000);
		switch(robot_operation_info.mode)
		{
			case ROBOT_OPERATION_MANUAL:
				robot_operation_manual_operation();
				break;
			case ROBOT_OPERATION_AUTO:
				robot_operation_auto_operation();
				break;
		}
	}
}

void robot_operation_init(fp_get_image_offset getImageOffset, fp_robot_turned robotTurned, fp_robot_moved robotMoved)
{
	pthread_t thread1;
	int x = 0;
	servos_manager_main();
	pthread_create(&thread1, NULL, &robot_operation_main, &x);
}

long micros_wrapper() {
#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.
	return micros();
#else
	return 0;
#endif // UBUNTU
}

