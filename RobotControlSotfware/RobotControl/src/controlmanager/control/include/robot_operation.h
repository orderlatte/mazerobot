#ifndef _ROBOT_OPERATION
#define _ROBOT_OPERATION

typedef enum
{
	ROBOT_OPERATION_DIRECTION_INIT = 0,

	ROBOT_OPERATION_DIRECTION_FORWARD,
	ROBOT_OPERATION_DIRECTION_BACKWARD,
	ROBOT_OPERATION_DIRECTION_RIGHT,
	ROBOT_OPERATION_DIRECTION_LEFT,
	ROBOT_OPERATION_DIRECTION_STOP,

	ROBOT_OPERATION_DIRECTION_MAX
} T_robot_operation_direction;

typedef enum
{
	ROBOT_OPERATION_MANUAL = 0,

	ROBOT_OPERATION_AUTO,
	ROBOT_OPERATION_SUSPEND,

	ROBOT_OPERATION_MAX
} T_robot_operation_mode;

typedef struct
{
	unsigned char robot_run;
	T_robot_operation_mode mode;
	T_robot_operation_direction direction;
}T_robot_operation_info;

void robot_operation_init(void);
void robot_operation_manual(T_robot_operation_direction direction);
void robot_operation_auto(T_robot_operation_direction direction);




#endif

