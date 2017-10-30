#ifndef _SENSOR_MANAGER
#define _SENSOR_MANAGER

typedef struct
{
	double sonar_distance;
	unsigned int laser_left_distance;
	unsigned int laser_right_distance;
}T_SensorData;

void sensor_manager_main(void);
T_SensorData get_sensor_data(void);





#endif
