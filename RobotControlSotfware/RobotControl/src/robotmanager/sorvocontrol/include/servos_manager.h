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

#endif

