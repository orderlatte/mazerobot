#ifndef _SERVOS_MANAGER
#define _SERVOS_MANAGER

typedef enum
{
	ROBOT_STOP = 0,

	ROBOT_LINE_TRACKING,
	ROBOT_FORWARD_MOVING,
	ROBOT_BACKWARD_MOVING,
	ROBOT_RIGHT_ROTATING,
	ROBOT_LEFT_ROTATING,

	ROBOT_MOVING_MODE_MAX
} T_robot_moving_mode;

void robot_mode_setting(T_robot_moving_mode robot_moving_mode, float offset);
void servos_manager_main(void);
void servos_cam_operation(int Pan, int Tilt);



// Parameters for PID
#define KP        1.0               // defaut 1.0
#define KI        0.03             // defaut 0.1
#define KD        0.001               // defaut 0.0
#define BASESPEED 10.0               // Range 0-50
#define M_BASESPEED 6.0               // Range 0-50

#define BASESPEEDFUDGEFACTOR 0.80   // default 1.25

#define MIN_WHEEL_SPEED -50
#define MAX_WHEEL_SPEED  50




#endif

