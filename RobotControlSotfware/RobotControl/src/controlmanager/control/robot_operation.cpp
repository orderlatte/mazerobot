#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <iostream>

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

T_robot_operation_info robot_operation_info;

static fp_get_image_offset fpGetImageOffset;
static fp_robot_turned fpRobotTurned;
static fp_robot_moved fpRobotMoved;

static T_robot_image_info robot_operation_image_info;

static int            Pan;
static int            Tilt;

T_robot_operation_direction forward_moving_avoid_wall;

static unsigned char auto_init_flag;

extern double pid_kp;
extern double pid_ki;
extern double pid_kd;
extern TPID PID;




long micros_wrapper();


void robot_move_one_cell_foward(T_robot_operation_direction direction)
{
	static unsigned char robot_move_one_cell_foward_state = 0;
	static unsigned long timeoutstart = 0;
	static unsigned long time_backup;

	if(auto_init_flag ==  1)
	{
		auto_init_flag = 0;
		robot_move_one_cell_foward_state = 0;
	}
	if(robot_operation_image_info.linewidth > 190)
	{
		robot_operation_image_info.offset = 0;
	}

	
	if(forward_moving_avoid_wall == ROBOT_OPERATION_DIRECTION_LEFT)
	{
		robot_mode_setting(ROBOT_LEFT_ROTATING,robot_operation_image_info.offset);
		usleep(100*1000);
		forward_moving_avoid_wall = ROBOT_OPERATION_DIRECTION_INIT;
		timeoutstart = timeoutstart + (micros_wrapper() - time_backup);
	}
	else if(forward_moving_avoid_wall == ROBOT_OPERATION_DIRECTION_RIGHT)
	{
		robot_mode_setting(ROBOT_RIGHT_ROTATING,robot_operation_image_info.offset);
		usleep(100*1000);
		forward_moving_avoid_wall = ROBOT_OPERATION_DIRECTION_INIT;
		timeoutstart = timeoutstart + (micros_wrapper() - time_backup);
	}
	else
	{
		time_backup = micros_wrapper();
		forward_moving_avoid_wall = ROBOT_OPERATION_DIRECTION_INIT;
	}

	if(robot_move_one_cell_foward_state == 0)
	{
		timeoutstart = micros_wrapper();
		robot_mode_setting(ROBOT_LINE_TRACKING, robot_operation_image_info.offset);
		robot_move_one_cell_foward_state = 1;
	}
	else if(robot_move_one_cell_foward_state == 1)
	{
		robot_mode_setting(ROBOT_LINE_TRACKING, robot_operation_image_info.offset);
		if(direction == ROBOT_OPERATION_DIRECTION_BACK_FORWARD)
		{
			if(micros_wrapper()-timeoutstart > (400*1000))
			{
				robot_move_one_cell_foward_state = 2;
			}
		}
		else
		{
			if(micros_wrapper()-timeoutstart > (600*1000))
			{
				robot_move_one_cell_foward_state = 2;
			}
		}
	}
	else if(robot_move_one_cell_foward_state == 2)
	{
		robot_mode_setting(ROBOT_LINE_TRACKING, robot_operation_image_info.offset);
		if(robot_operation_image_info.linewidth < 190 && robot_operation_image_info.linewidth > 100)
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
		if(micros_wrapper()-timeoutstart < (600*1000))
		{
			robot_mode_setting(ROBOT_LINE_TRACKING, robot_operation_image_info.offset);
		}
		else
		{
//			robot_mode_setting(ROBOT_STOP, robot_operation_image_info.offset);
			robot_move_one_cell_foward_state = 0;
			robot_operation_info.robot_run = 0;
			fpRobotMoved();
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
			robot_mode_setting(ROBOT_LEFT_ROTATING,robot_operation_image_info.offset);
		}
		else
		{
			robot_mode_setting(ROBOT_RIGHT_ROTATING,robot_operation_image_info.offset);
		}

		if(micros_wrapper()-timeoutstart > (700*1000))
		{
			if((robot_operation_image_info.linewidth < 190 && robot_operation_image_info.linewidth > 100) || (micros_wrapper()-timeoutstart > (1000*1000)))
			{
//				robot_mode_setting(ROBOT_STOP, robot_operation_image_info.offset);
				robot_turn_to_cross_state = 0;
				robot_operation_info.robot_run = 0;
				fpRobotTurned();
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
		SetWheelSpeed(-10,0);
		if(micros_wrapper()-timeoutstart > (800*1000))
		{
			timeoutstart = micros_wrapper();
			robot_turn_to_cross_state = 2;
		}
	}	
	else if(robot_turn_to_cross_state == 2)
	{
		//robot_mode_setting(ROBOT_LEFT_ROTATING,offset);
		robot_mode_setting(ROBOT_LEFT_ROTATING,robot_operation_image_info.offset);
		if(micros_wrapper()-timeoutstart > (1000*1000))
		{
			robot_turn_to_cross_state = 3;
			timeoutstart = micros_wrapper();
		}
	}
	else if(robot_turn_to_cross_state == 3)
	{
		if((robot_operation_image_info.linewidth < 190 && robot_operation_image_info.linewidth > 100) || (micros_wrapper()-timeoutstart > (500*1000)))
		{
//			robot_mode_setting(ROBOT_STOP, robot_operation_image_info.offset);
//			robot_operation_info.direction = ROBOT_OPERATION_DIRECTION_FORWARD;
			robot_turn_to_cross_state = 0;
			fpRobotTurned();
		}
	}
	
}

void robot_operation_manual(T_robot_operation_direction direction)
{
	robot_operation_info.direction = direction;
	robot_operation_info.robot_run = 1;
	robot_operation_info.mode = ROBOT_OPERATION_MANUAL;
	auto_init_flag = 1;
}

void robot_operation_auto(T_robot_operation_direction direction)
{
	robot_operation_info.direction = direction;
	robot_operation_info.robot_run = 1;
	robot_operation_info.mode = ROBOT_OPERATION_AUTO;
	InitPID(PID,pid_kp,pid_ki,pid_kd,BASESPEED,BASESPEEDFUDGEFACTOR) ; //initialize the PID
}

void robot_operation_meet_wall(T_robot_operation_direction direction)
{
	forward_moving_avoid_wall = direction;
}


void robot_operation_manual_operation(void)
{
	if(robot_operation_info.robot_run == 1)
	{
		switch(robot_operation_info.direction)
		{
			case ROBOT_OPERATION_DIRECTION_FORWARD:
				robot_mode_setting(ROBOT_FORWARD_MOVING, robot_operation_image_info.offset);
				break;
			case ROBOT_OPERATION_DIRECTION_LEFT:
				robot_mode_setting(ROBOT_LEFT_ROTATING,robot_operation_image_info.offset);
				break;
			case ROBOT_OPERATION_DIRECTION_RIGHT:
				robot_mode_setting(ROBOT_RIGHT_ROTATING,robot_operation_image_info.offset);
				break;
			case ROBOT_OPERATION_DIRECTION_BACKWARD:
				robot_mode_setting(ROBOT_BACKWARD_MOVING, robot_operation_image_info.offset);
				break;
			default:
				robot_mode_setting(ROBOT_STOP, robot_operation_image_info.offset);
				robot_operation_info.robot_run = 0;
				break;
		}
	}
}

void robot_operation_auto_operation(void)
{
	if(robot_operation_info.robot_run == 1)
	{
		switch(robot_operation_info.direction)
		{
			case ROBOT_OPERATION_DIRECTION_FORWARD:
			case ROBOT_OPERATION_DIRECTION_BACK_FORWARD:
				robot_move_one_cell_foward(robot_operation_info.direction);
				break;
			case ROBOT_OPERATION_DIRECTION_LEFT:
			case ROBOT_OPERATION_DIRECTION_RIGHT:
				robot_trun_move_onecell(robot_operation_info.direction);
				break;
			case ROBOT_OPERATION_DIRECTION_BACKWARD:
				robot_back_move_one_cell();
				break;
			default:
				robot_mode_setting(ROBOT_STOP, robot_operation_image_info.offset);
				robot_operation_info.robot_run = 0;
				break;
				
		}
	}
				
}


void robot_operation_cam_position(int pan, int tilt)
{
	Pan = pan;
	Tilt = tilt;
	servos_cam_operation(pan, tilt);
}


void robot_operation_cam_manual(T_robot_operation_direction direction)
{
	switch(direction)
	{
		case ROBOT_CAM_DIRECTION_LEFT:
			if(Pan < PAN_CAMERA_MAX)
			{
				Pan++;
			}
			break;
		case ROBOT_CAM_DIRECTION_RIGHT:
			if(Pan > PAN_CAMERA_MIN)
			{
				Pan--;
			}
			break;
		case ROBOT_CAM_DIRECTION_DOWN:
			if(Tilt < TILT_CAMERA_MAX)
			{
				Tilt++;
			}
			break;
		case ROBOT_CAM_DIRECTION_UP:
			if(Tilt > TILT_CAMERA_MIN)
			{
				Tilt--;
			}
			break;
		case ROBOT_CAM_DIRECTION_CENTER:
			Pan = 155;
			Tilt = 155;
			break;
		case ROBOT_CAM_DIRECTION_LEFT_SIGN:
			Pan = 220;
			Tilt = 150;
			break;
		case ROBOT_CAM_DIRECTION_RIGHT_SIGN:
			Pan = 72;
			Tilt = 167;
			break;
		case ROBOT_CAM_DIRECTION_LINE:
			Pan = TRK_LINE_CAM_PAN;
			Tilt = TRK_LINE_CAM_TILT;
			break;
			
	}
	robot_operation_cam_position(Pan, Tilt);
	printf("pan = %d, tilt = %d\n", Pan, Tilt);
			
}


void *robot_operation_main(void *value)
{
	while(1)
	{
		usleep(5*1000);
		robot_operation_image_info = fpGetImageOffset();
		
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
	Pan = TRK_LINE_CAM_PAN;
	Tilt = TRK_LINE_CAM_TILT;
	servos_manager_main();
	robot_operation_cam_position(Pan, Tilt);
	pthread_create(&thread1, NULL, &robot_operation_main, &x);

	fpGetImageOffset = getImageOffset;
	fpRobotTurned = robotTurned;
	fpRobotMoved = robotMoved;
}

long micros_wrapper() {
#ifndef UBUNTU		// For building in ubuntu. Below code sould be built in raspberry pi.
	return micros();
#else
	return 0;
#endif // UBUNTU
}

