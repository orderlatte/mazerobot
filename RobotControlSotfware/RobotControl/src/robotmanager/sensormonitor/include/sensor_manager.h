#ifndef _SENSOR_MANAGER
#define _SENSOR_MANAGER

typedef struct
{
	double sonar_distance;
	unsigned int laser_left_distance;
	unsigned int laser_right_distance;
}T_SensorData;

typedef struct
{
	int wall_forward;	// 1: Wall is existed, 0: Wall is not existed.
	int wall_right;
	int wall_left;
	int hit_wall_forward;	// 1: Robot may hit a wall, 0: Robot may not hit a wall.
	int hit_wall_right;
	int hit_Wall_left;
}T_CellData;

typedef void (*fp_robot_stop)(int);	// Parameter 1: Forward, 2: right, 3: left

void sensor_manager_main(fp_robot_stop fpstop);
T_SensorData get_sensor_data(void);





#endif
