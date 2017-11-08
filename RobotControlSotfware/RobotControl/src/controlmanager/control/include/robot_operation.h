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

typedef struct
{
	int linewidth;
	float offset;
}T_robot_image_info;

typedef T_robot_image_info (*fp_get_image_offset)();
typedef void (*fp_robot_turned)();
typedef void (*fp_robot_moved)();


void robot_operation_init(fp_get_image_offset getImageOffset, fp_robot_turned robotTurned, fp_robot_moved robotMoved);
void robot_operation_manual(T_robot_operation_direction direction);
void robot_operation_auto(T_robot_operation_direction direction);






#endif

