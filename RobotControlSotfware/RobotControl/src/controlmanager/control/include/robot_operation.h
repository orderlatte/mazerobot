#ifndef _ROBOT_OPERATION
#define _ROBOT_OPERATION

#define TRK_LINE_CAM_PAN     155 //157
#define TRK_LINE_CAM_TILT    210 //192

typedef enum
{
	ROBOT_OPERATION_DIRECTION_INIT = 0,

	ROBOT_OPERATION_DIRECTION_FORWARD,
	ROBOT_OPERATION_DIRECTION_BACKWARD,
	ROBOT_OPERATION_DIRECTION_RIGHT,
	ROBOT_OPERATION_DIRECTION_LEFT,
	ROBOT_CAM_DIRECTION_UP,
	ROBOT_CAM_DIRECTION_DOWN,
	ROBOT_CAM_DIRECTION_LEFT,
	ROBOT_CAM_DIRECTION_RIGHT,
	ROBOT_CAM_DIRECTION_CENTER,
	ROBOT_CAM_DIRECTION_RIGHT_SIGN,
	ROBOT_CAM_DIRECTION_LEFT_SIGN,
	ROBOT_CAM_DIRECTION_LINE,
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
void robot_operation_cam_manual(T_robot_operation_direction direction);
void robot_operation_meet_wall(T_robot_operation_direction direction);








#endif

